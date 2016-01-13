#ifdef _DEBUG
#pragma comment(lib, "TestUtilsd.lib")
#else
#pragma comment(lib, "TestUtils.lib")
#endif

#include "../TestUtils/DB_Log.h"
#include "../TestUtils/DB_Timer.h"

#include <iostream>
#include <d3dx9.h>

using namespace std;
using namespace DaBianYLK;

/*
* 测试目的：比较常用DX函数与自己实现函数的效率
*/

const unsigned uCalculationCount = 20000000;

//////////////////////////////////////////////////// Quaternion ///////////////////////////////////////////////////////////////

D3DXVECTOR3* g_pAxis;
float* g_pAngle;
D3DXQUATERNION* g_pQuaternion;

inline D3DXQUATERNION* AxisAngleToQuaternion(D3DXQUATERNION* pQuaternion, const D3DXVECTOR3* pAxis, float angle)
{
	float halfRadian = angle * 0.5f;
	float fSin = sin(halfRadian);

	pQuaternion->w = cos(halfRadian);		// W = cos(radian / 2)
	pQuaternion->x = fSin * pAxis->x;		// X = sin(radian / 2) * rotateAxis.x
	pQuaternion->y = fSin * pAxis->y;		// Y = sin(radian / 2) * rotateAxis.y
	pQuaternion->z = fSin * pAxis->z;		// Z = sin(radian / 2) * rotateAxis.z

	return pQuaternion;
}

inline void QuaternionToAxisAngle(const D3DXQUATERNION* pQuaternion, D3DXVECTOR3* pAxis, float* pAngle)
{
	float x = pQuaternion->x;
	float y = pQuaternion->y;
	float z = pQuaternion->z;
	float fSqrLength = x*x + y*y + z*z;

	if (fSqrLength > 0.0)
	{
		float w = pQuaternion->w;
		*pAngle = 2.0f * acos(w);
		float fInvLength = 1.0f / sqrt(fSqrLength);
		pAxis->x = x * fInvLength;
		pAxis->y = y * fInvLength;
		pAxis->z = z * fInvLength;
	}
	else
	{
		// angle is 0 (mod 2*pi), so any axis will do
		*pAngle = 0.0f;
		pAxis->x = 1.0f;
		pAxis->y = 0.0f;
		pAxis->z = 0.0f;
	}
}

void DXQuaternionCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		D3DXQuaternionRotationAxis(g_pQuaternion, g_pAxis, *g_pAngle);
		D3DXQuaternionToAxisAngle(g_pQuaternion, g_pAxis, g_pAngle);
	}
}

void MyQuaternionCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		AxisAngleToQuaternion(g_pQuaternion, g_pAxis, *g_pAngle);
		QuaternionToAxisAngle(g_pQuaternion, g_pAxis, g_pAngle);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////// Matrix ///////////////////////////////////////////////////////////////
D3DXMATRIX* g_pMatrix;
D3DXVECTOR3* g_pPosition;

inline D3DXVECTOR3* VectorTransform(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix)
{
	float x = pInVector->x;
	float y = pInVector->y;
	float z = pInVector->z;

	pOutVector->x = (*pMatrix)(0, 0) * x + (*pMatrix)(1, 0) * y + (*pMatrix)(2, 0) * z + (*pMatrix)(3, 0);
	pOutVector->y = (*pMatrix)(0, 1) * x + (*pMatrix)(1, 1) * y + (*pMatrix)(2, 1) * z + (*pMatrix)(3, 1);
	pOutVector->z = (*pMatrix)(0, 2) * x + (*pMatrix)(1, 2) * y + (*pMatrix)(2, 2) * z + (*pMatrix)(3, 2);

	return pOutVector;
}

/*
关于SIMD指令个人的几点发现：
1. 当使用的xmm寄存器不冲突时，相同的运算指令是可以并行的, 如_mm_mul_ss(xmm0, xmm1)可以与_mm_mul_ss(xmm2, xmm3)并行
2. _mm_store_ss指令不能并行，120亿次_mm_store_ss指令约消耗3.5s左右的CPU时间
3. _mm_load_ss和_mm_set_ss的性能消耗很低，与_mm_store_ss相比可以忽略不计（比值1%以下，可能与CPU寄存器有关）
4. _mm_loadu_ps与_mm_set_ps的性能开销几乎一样（可能受到CPU寄存器的影响）
5. SIMD指令执行向量计算时不一定比向量的每个元素单独计算更快（）
6. 使用_aligned_malloc申请16字节对齐的内存空间时效率很低
7. 自己整理的SIMD指令比VS2013翻译的指令效率高5%-10%左右，与DX函数相比效率提高了50%以上
*/
//#define USE_VECTOR_COMPUTE
inline D3DXVECTOR3* SimdVectorTransform(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix)
{
#ifdef USE_VECTOR_COMPUTE
	// _mm_set_ps(float z, float y, float x, float w)对应于_mm_load_ps的功能，参数是四个单独的单精度浮点数，不需要字节对齐，需要多条指令。（r0=w, r1 = x, r2 = y, r3 = z，注意顺序）
	__m128 InX = _mm_set_ps(0.0f, pInVector->x, pInVector->x, pInVector->x);
	__m128 InY = _mm_set_ps(0.0f, pInVector->y, pInVector->y, pInVector->y);
	__m128 InZ = _mm_set_ps(0.0f, pInVector->z, pInVector->z, pInVector->z);
	__m128 MatrixRow1 = _mm_loadu_ps(pMatrix->m[0]);
	__m128 MatrixRow2 = _mm_loadu_ps(pMatrix->m[1]);
	__m128 MatrixRow3 = _mm_loadu_ps(pMatrix->m[2]);
	__m128 MatrixRow4 = _mm_loadu_ps(pMatrix->m[3]);
	/*__m128 MatrixRow1 = _mm_set_ps(0.0f, pMatrix->_13, pMatrix->_12, pMatrix->_11);
	__m128 MatrixRow2 = _mm_set_ps(0.0f, pMatrix->_23, pMatrix->_22, pMatrix->_21);
	__m128 MatrixRow3 = _mm_set_ps(0.0f, pMatrix->_33, pMatrix->_32, pMatrix->_31);
	__m128 MatrixRow4 = _mm_set_ps(0.0f, pMatrix->_43, pMatrix->_42, pMatrix->_41);*/

	InX = _mm_mul_ps(InX, MatrixRow1);
	InY = _mm_mul_ps(InY, MatrixRow2);
	InZ = _mm_mul_ps(InZ, MatrixRow3);

	InX = _mm_add_ps(InX, MatrixRow4);
	InY = _mm_add_ps(InY, InZ);

	InX = _mm_add_ps(InX, InY);

	D3DXVECTOR4 temp;
	_mm_store_ps(reinterpret_cast<float*>(&temp), InX);

	pOutVector->x = temp.x;
	pOutVector->y = temp.y;
	pOutVector->z = temp.z;

	// 16字节对齐的_aligned_malloc效率很低
	/*void* pOutBuffer = _aligned_malloc(16, 16);
	_mm_store_ps(reinterpret_cast<float*>(pOutBuffer), InX);
	memcpy(pOutVector, pOutBuffer, sizeof(float)* 3);
	_aligned_free(pOutBuffer);*/
#else
	__m128 InX = _mm_set_ss(pInVector->x);
	__m128 InY = _mm_set_ss(pInVector->y);
	__m128 InZ = _mm_set_ss(pInVector->z);

	__m128 Mat1 = _mm_set_ss(pMatrix->_11);
	__m128 Mat2 = _mm_set_ss(pMatrix->_21);
	__m128 Mat3 = _mm_set_ss(pMatrix->_31);
	__m128 Mat4 = _mm_set_ss(pMatrix->_41);

	Mat1 = _mm_mul_ss(Mat1, InX);
	Mat2 = _mm_mul_ss(Mat2, InY);
	Mat3 = _mm_mul_ss(Mat3, InZ);
	Mat1 = _mm_add_ss(Mat1, Mat2);
	Mat3 = _mm_add_ss(Mat3, Mat4);
	Mat1 = _mm_add_ss(Mat1, Mat3);

	_mm_store_ss(&pOutVector->x, Mat1);

	Mat1 = _mm_set_ss(pMatrix->_12);
	Mat2 = _mm_set_ss(pMatrix->_22);
	Mat3 = _mm_set_ss(pMatrix->_32);
	Mat4 = _mm_set_ss(pMatrix->_42);

	Mat1 = _mm_mul_ss(Mat1, InX);
	Mat2 = _mm_mul_ss(Mat2, InY);
	Mat3 = _mm_mul_ss(Mat3, InZ);
	Mat1 = _mm_add_ss(Mat1, Mat2);
	Mat3 = _mm_add_ss(Mat3, Mat4);
	Mat1 = _mm_add_ss(Mat1, Mat3);

	_mm_store_ss(&pOutVector->y, Mat1);

	Mat1 = _mm_set_ss(pMatrix->_13);
	Mat2 = _mm_set_ss(pMatrix->_23);
	Mat3 = _mm_set_ss(pMatrix->_33);
	Mat4 = _mm_set_ss(pMatrix->_43);

	Mat1 = _mm_mul_ss(Mat1, InX);
	Mat2 = _mm_mul_ss(Mat2, InY);
	Mat3 = _mm_mul_ss(Mat3, InZ);
	Mat1 = _mm_add_ss(Mat1, Mat2);
	Mat3 = _mm_add_ss(Mat3, Mat4);
	Mat1 = _mm_add_ss(Mat1, Mat3);

	_mm_store_ss(&pOutVector->z, Mat1);
#endif

	return pOutVector;
}

// 根据VS2013编译的汇编指令翻译出来的SIMD代码
inline D3DXVECTOR3* AsmVectorTransform(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix)
{
	__m128 xmm0;
	__m128 xmm1;
	__m128 xmm2;
	__m128 xmm3;

	__m128 xmm4;
	__m128 xmm5;
	__m128 xmm6;

	__m128 xmm7 = _mm_load_ss(&pMatrix->_21);

	xmm6 = _mm_load_ss(&pInVector->x);
	xmm5 = _mm_load_ss(&pInVector->y);
	xmm4 = _mm_load_ss(&pInVector->z);

	xmm1 = xmm6;
	xmm6 = _mm_load_ss(&pMatrix->_11);
	xmm6 = _mm_mul_ss(xmm6, xmm1);
	xmm2 = xmm5;
	xmm5 = _mm_load_ss(&pMatrix->_12);
	xmm0 = xmm7;
	xmm0 = _mm_mul_ss(xmm0, xmm2);
	xmm3 = xmm4;
	xmm4 = _mm_load_ss(&pMatrix->_13);
	xmm5 = _mm_mul_ss(xmm5, xmm1);
	xmm4 = _mm_mul_ss(xmm4, xmm1);
	xmm6 = _mm_add_ss(xmm6, xmm0);
	xmm0 = _mm_load_ss(&pMatrix->_31);
	xmm0 = _mm_mul_ss(xmm0, xmm3);
	xmm6 = _mm_add_ss(xmm6, xmm0);
	xmm0 = _mm_load_ss(&pMatrix->_22);
	xmm0 = _mm_mul_ss(xmm0, xmm2);
	xmm5 = _mm_add_ss(xmm5, xmm0);
	xmm0 = _mm_load_ss(&pMatrix->_32);
	xmm0 = _mm_mul_ss(xmm0, xmm3);
	xmm7 = _mm_load_ss(&pMatrix->_41);
	xmm6 = _mm_add_ss(xmm6, xmm7);
	xmm5 = _mm_add_ss(xmm5, xmm0);
	xmm0 = _mm_load_ss(&pMatrix->_23);
	xmm0 = _mm_mul_ss(xmm0, xmm2);
	xmm4 = _mm_add_ss(xmm4, xmm0);
	xmm0 = _mm_load_ss(&pMatrix->_33);
	xmm0 = _mm_mul_ss(xmm0, xmm3);
	xmm7 = _mm_load_ss(&pMatrix->_42);
	xmm5 = _mm_add_ss(xmm5, xmm7);
	xmm4 = _mm_add_ss(xmm4, xmm0);
	xmm7 = _mm_load_ss(&pMatrix->_43);
	xmm4 = _mm_add_ss(xmm4, xmm7);

	_mm_store_ss(&pOutVector->z, xmm4);
	_mm_store_ss(&pOutVector->y, xmm5);
	_mm_store_ss(&pOutVector->x, xmm6);

	return pOutVector;
}

void DXVectorTransformCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		D3DXVec3TransformCoord(g_pPosition, g_pPosition, g_pMatrix);
	}
}

void MyVectorTransformCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		VectorTransform(g_pPosition, g_pPosition, g_pMatrix);
	}
}

void AsmVectorTransformCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		AsmVectorTransform(g_pPosition, g_pPosition, g_pMatrix);
	}
}

void SimdVectorTransformCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		SimdVectorTransform(g_pPosition, g_pPosition, g_pMatrix);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	// 在堆分配参数，防止编译器对取内存地址的过程进行优化影响对比结果（从静态空间取地址比从动态堆空间取地址效率高50%以上）
	g_pAxis = new D3DXVECTOR3(0.5773502f, 0.5773502f, 0.5773502f);
	g_pAngle = new float(1.0f);
	g_pQuaternion = new D3DXQUATERNION();
	BENCHMARK(DXQuaternionCompute, DXQuaternionCompute());
	Log("Result: (%f, %f, %f, %f)", g_pQuaternion->w, g_pQuaternion->x, g_pQuaternion->y, g_pQuaternion->z); // 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	delete g_pAxis;
	delete g_pAngle;
	delete g_pQuaternion;

	g_pAxis = new D3DXVECTOR3(0.5773502f, 0.5773502f, 0.5773502f);
	g_pAngle = new float(1.0f);
	g_pQuaternion = new D3DXQUATERNION();

	BENCHMARK(MyQuaternionCompute, MyQuaternionCompute());
	Log("Result: (%f, %f, %f, %f)", g_pQuaternion->w, g_pQuaternion->x, g_pQuaternion->y, g_pQuaternion->z); // 这里要输出一次result,否则编译器的优化会删除执行运算的代码

	delete g_pAxis;
	delete g_pAngle;
	delete g_pQuaternion;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	g_pMatrix = new D3DXMATRIX();
	D3DXMatrixLookAtLH(g_pMatrix, new D3DXVECTOR3(33.0f, 33.0f, 33.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 10.0f, 0.0f));
	g_pPosition = new D3DXVECTOR3(5.0f, 5.0f, 5.0f);

	BENCHMARK(DXVectorTransformCompute, DXVectorTransformCompute());
	Log("Result: (%f, %f, %f)", g_pPosition->x, g_pPosition->y, g_pPosition->z); // 这里要输出一次result,否则编译器的优化会删除执行运算的代码

	delete g_pMatrix;
	delete g_pPosition;

	g_pMatrix = new D3DXMATRIX();
	D3DXMatrixLookAtLH(g_pMatrix, new D3DXVECTOR3(33.0f, 33.0f, 33.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 10.0f, 0.0f));
	g_pPosition = new D3DXVECTOR3(5.0f, 5.0f, 5.0f);

	BENCHMARK(MyVectorTransformCompute, MyVectorTransformCompute());
	Log("Result: (%f, %f, %f)", g_pPosition->x, g_pPosition->y, g_pPosition->z); // 这里要输出一次result,否则编译器的优化会删除执行运算的代码

	delete g_pMatrix;
	delete g_pPosition;

	g_pMatrix = new D3DXMATRIX();
	D3DXMatrixLookAtLH(g_pMatrix, new D3DXVECTOR3(33.0f, 33.0f, 33.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 10.0f, 0.0f));
	g_pPosition = new D3DXVECTOR3(5.0f, 5.0f, 5.0f);

	BENCHMARK(AsmVectorTransformCompute, AsmVectorTransformCompute());
	Log("Result: (%f, %f, %f)", g_pPosition->x, g_pPosition->y, g_pPosition->z); // 这里要输出一次result,否则编译器的优化会删除执行运算的代码

	delete g_pMatrix;
	delete g_pPosition;

	g_pMatrix = new D3DXMATRIX();
	D3DXMatrixLookAtLH(g_pMatrix, new D3DXVECTOR3(33.0f, 33.0f, 33.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 10.0f, 0.0f));
	g_pPosition = new D3DXVECTOR3(5.0f, 5.0f, 5.0f);

	BENCHMARK(SimdVectorTransformCompute, SimdVectorTransformCompute());
	Log("Result: (%f, %f, %f)", g_pPosition->x, g_pPosition->y, g_pPosition->z); // 这里要输出一次result,否则编译器的优化会删除执行运算的代码

	delete g_pMatrix;
	delete g_pPosition;

	system("pause");

	return 0;
}
