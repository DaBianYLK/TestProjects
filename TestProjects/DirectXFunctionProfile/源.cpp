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
* ����Ŀ�ģ��Ƚϳ���DX�������Լ�ʵ�ֺ�����Ч��
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
����SIMDָ����˵ļ��㷢�֣�
1. ��ʹ�õ�xmm�Ĵ�������ͻʱ����ͬ������ָ���ǿ��Բ��е�, ��_mm_mul_ss(xmm0, xmm1)������_mm_mul_ss(xmm2, xmm3)����
2. _mm_store_ssָ��ܲ��У�120�ڴ�_mm_store_ssָ��Լ����3.5s���ҵ�CPUʱ��
3. _mm_load_ss��_mm_set_ss���������ĺܵͣ���_mm_store_ss��ȿ��Ժ��Բ��ƣ���ֵ1%���£�������CPU�Ĵ����йأ�
4. _mm_loadu_ps��_mm_set_ps�����ܿ�������һ���������ܵ�CPU�Ĵ�����Ӱ�죩
5. SIMDָ��ִ����������ʱ��һ����������ÿ��Ԫ�ص���������죨��
6. ʹ��_aligned_malloc����16�ֽڶ�����ڴ�ռ�ʱЧ�ʺܵ�
7. �Լ������SIMDָ���VS2013�����ָ��Ч�ʸ�5%-10%���ң���DX�������Ч�������50%����
*/
//#define USE_VECTOR_COMPUTE
inline D3DXVECTOR3* SimdVectorTransform(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix)
{
#ifdef USE_VECTOR_COMPUTE
	// _mm_set_ps(float z, float y, float x, float w)��Ӧ��_mm_load_ps�Ĺ��ܣ��������ĸ������ĵ����ȸ�����������Ҫ�ֽڶ��룬��Ҫ����ָ���r0=w, r1 = x, r2 = y, r3 = z��ע��˳��
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

	// 16�ֽڶ����_aligned_mallocЧ�ʺܵ�
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

// ����VS2013����Ļ��ָ��������SIMD����
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
	// �ڶѷ����������ֹ��������ȡ�ڴ��ַ�Ĺ��̽����Ż�Ӱ��ԱȽ�����Ӿ�̬�ռ�ȡ��ַ�ȴӶ�̬�ѿռ�ȡ��ַЧ�ʸ�50%���ϣ�
	g_pAxis = new D3DXVECTOR3(0.5773502f, 0.5773502f, 0.5773502f);
	g_pAngle = new float(1.0f);
	g_pQuaternion = new D3DXQUATERNION();
	BENCHMARK(DXQuaternionCompute, DXQuaternionCompute());
	Log("Result: (%f, %f, %f, %f)", g_pQuaternion->w, g_pQuaternion->x, g_pQuaternion->y, g_pQuaternion->z); // ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	delete g_pAxis;
	delete g_pAngle;
	delete g_pQuaternion;

	g_pAxis = new D3DXVECTOR3(0.5773502f, 0.5773502f, 0.5773502f);
	g_pAngle = new float(1.0f);
	g_pQuaternion = new D3DXQUATERNION();

	BENCHMARK(MyQuaternionCompute, MyQuaternionCompute());
	Log("Result: (%f, %f, %f, %f)", g_pQuaternion->w, g_pQuaternion->x, g_pQuaternion->y, g_pQuaternion->z); // ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	delete g_pAxis;
	delete g_pAngle;
	delete g_pQuaternion;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	g_pMatrix = new D3DXMATRIX();
	D3DXMatrixLookAtLH(g_pMatrix, new D3DXVECTOR3(33.0f, 33.0f, 33.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 10.0f, 0.0f));
	g_pPosition = new D3DXVECTOR3(5.0f, 5.0f, 5.0f);

	BENCHMARK(DXVectorTransformCompute, DXVectorTransformCompute());
	Log("Result: (%f, %f, %f)", g_pPosition->x, g_pPosition->y, g_pPosition->z); // ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	delete g_pMatrix;
	delete g_pPosition;

	g_pMatrix = new D3DXMATRIX();
	D3DXMatrixLookAtLH(g_pMatrix, new D3DXVECTOR3(33.0f, 33.0f, 33.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 10.0f, 0.0f));
	g_pPosition = new D3DXVECTOR3(5.0f, 5.0f, 5.0f);

	BENCHMARK(MyVectorTransformCompute, MyVectorTransformCompute());
	Log("Result: (%f, %f, %f)", g_pPosition->x, g_pPosition->y, g_pPosition->z); // ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	delete g_pMatrix;
	delete g_pPosition;

	g_pMatrix = new D3DXMATRIX();
	D3DXMatrixLookAtLH(g_pMatrix, new D3DXVECTOR3(33.0f, 33.0f, 33.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 10.0f, 0.0f));
	g_pPosition = new D3DXVECTOR3(5.0f, 5.0f, 5.0f);

	BENCHMARK(AsmVectorTransformCompute, AsmVectorTransformCompute());
	Log("Result: (%f, %f, %f)", g_pPosition->x, g_pPosition->y, g_pPosition->z); // ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	delete g_pMatrix;
	delete g_pPosition;

	g_pMatrix = new D3DXMATRIX();
	D3DXMatrixLookAtLH(g_pMatrix, new D3DXVECTOR3(33.0f, 33.0f, 33.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 10.0f, 0.0f));
	g_pPosition = new D3DXVECTOR3(5.0f, 5.0f, 5.0f);

	BENCHMARK(SimdVectorTransformCompute, SimdVectorTransformCompute());
	Log("Result: (%f, %f, %f)", g_pPosition->x, g_pPosition->y, g_pPosition->z); // ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	delete g_pMatrix;
	delete g_pPosition;

	system("pause");

	return 0;
}
