#include "MyDXFuntion.h"

D3DXQUATERNION* AxisAngleToQuaternion(D3DXQUATERNION* pQuaternion, const D3DXVECTOR3* pAxis, float angle)
{
	float halfRadian = angle * 0.5f;
	float fSin = sin(halfRadian);

	pQuaternion->w = cos(halfRadian);		// W = cos(radian / 2)
	pQuaternion->x = fSin * pAxis->x;		// X = sin(radian / 2) * rotateAxis.x
	pQuaternion->y = fSin * pAxis->y;		// Y = sin(radian / 2) * rotateAxis.y
	pQuaternion->z = fSin * pAxis->z;		// Z = sin(radian / 2) * rotateAxis.z

	return pQuaternion;
}

void QuaternionToAxisAngle(const D3DXQUATERNION* pQuaternion, D3DXVECTOR3* pAxis, float* pAngle)
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3DXVECTOR3* VectorTransform(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix)
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
D3DXVECTOR3* SimdVectorTransformParallel(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix)
{
	// _mm_set_ps(float z, float y, float x, float w)对应于_mm_load_ps的功能，参数是四个单独的单精度浮点数，不需要字节对齐，需要多条指令。
	//（r0=w, r1 = x, r2 = y, r3 = z，注意顺序）
	/*__m128 InX = _mm_set_ps(0.0f, pInVector->x, pInVector->x, pInVector->x);
	__m128 InY = _mm_set_ps(0.0f, pInVector->y, pInVector->y, pInVector->y);
	__m128 InZ = _mm_set_ps(0.0f, pInVector->z, pInVector->z, pInVector->z);*/
	__m128 InX = _mm_set1_ps(pInVector->x);
	__m128 InY = _mm_set1_ps(pInVector->y);
	__m128 InZ = _mm_set1_ps(pInVector->z);
	//__m128 InW = _mm_set1_ps(1.0f);
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
	//InW = _mm_mul_ps(InW, MatrixRow4);

	InX = _mm_add_ps(InX, MatrixRow4);
	InY = _mm_add_ps(InY, InZ);

	InX = _mm_add_ps(InX, InY);

	pOutVector->x = InX.m128_f32[0];
	pOutVector->y = InX.m128_f32[1];
	pOutVector->z = InX.m128_f32[2];

	return pOutVector;
}

	// 16字节对齐的_aligned_malloc效率很低
	/*void* pOutBuffer = _aligned_malloc(16, 16);
	_mm_store_ps(reinterpret_cast<float*>(pOutBuffer), InX);
	memcpy(pOutVector, pOutBuffer, sizeof(float)* 3);
	_aligned_free(pOutBuffer);*/
D3DXVECTOR3* SimdVectorTransformUnparallel(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix)
{
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

	return pOutVector;
}

SimdVector3* PackedSimdVectorTransform(SimdVector3* pOut, const SimdVector3* pIn, const SimdMatrix* pM)
{
	__m128 InX = _mm_set1_ps(pIn->x);
	__m128 InY = _mm_set1_ps(pIn->y);
	__m128 InZ = _mm_set1_ps(pIn->z);
	__m128 MatrixRow1 = _mm_load_ps(pM->m[0]);
	__m128 MatrixRow2 = _mm_load_ps(pM->m[1]);
	__m128 MatrixRow3 = _mm_load_ps(pM->m[2]);
	__m128 MatrixRow4 = _mm_load_ps(pM->m[3]);

	InX = _mm_mul_ps(InX, MatrixRow1);
	InY = _mm_mul_ps(InY, MatrixRow2);
	InZ = _mm_mul_ps(InZ, MatrixRow3);

	InX = _mm_add_ps(InX, MatrixRow4);
	InY = _mm_add_ps(InY, InZ);

	InX = _mm_add_ps(InX, InY);

	pOut->x = InX.m128_f32[0];
	pOut->y = InX.m128_f32[1];
	pOut->z = InX.m128_f32[2];

	return pOut;
}

SimdVector3* PackedAsmVectorTransform(SimdVector3* pOut, const SimdVector3* pIn, const SimdMatrix* pM)
{
	_asm
	{
		push	ebx
		push	ecx

		mov		eax, pOut
		mov		ebx, pIn
		mov		ecx, pM

		movaps	xmm0, [ebx]
		movaps	xmm1, xmm0
		movaps	xmm2, xmm0

		shufps	xmm0, xmm0, 0
		shufps  xmm1, xmm1, 85
		shufps  xmm2, xmm2, 170

		movaps	xmm3, [ecx]
		movaps	xmm4, [ecx + 10h]
		movaps	xmm5, [ecx + 20h]
		movaps	xmm6, [ecx + 30h]

		mulps	xmm0, xmm3
		mulps	xmm1, xmm4
		mulps	xmm2, xmm5

		addps	xmm0, xmm6
		addps	xmm1, xmm2

		addps	xmm0, xmm1

		movaps	[eax], xmm0

		pop	ebx
		pop	ecx
	}
}

D3DXVECTOR3* AsmVectorTransform(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix)
{
	_asm
	{
		push	ebx
		push	ecx

		mov		eax, pOutVector
		mov		ebx, pInVector
		mov		ecx, pMatrix

		movss	xmm0, [ebx]
		movss	xmm1, [ebx + 4]
		movss	xmm2, [ebx + 8]

		/////////////////////////////

		movss	xmm3, [ecx]
		movss	xmm4, [ecx + 10h]
		movss	xmm5, [ecx + 20h]
		movss	xmm6, [ecx + 30h]

		mulss	xmm3, xmm0
		mulss	xmm4, xmm1
		mulss	xmm5, xmm2

		addss	xmm3, xmm4
		addss	xmm5, xmm6

		addss	xmm3, xmm5

		movss[eax], xmm3

		/////////////////////////////

		movss	xmm3, [ecx + 4]
		movss	xmm4, [ecx + 14h]
		movss	xmm5, [ecx + 24h]
		movss	xmm6, [ecx + 34h]

		mulss	xmm3, xmm0
		mulss	xmm4, xmm1
		mulss	xmm5, xmm2

		addss	xmm3, xmm4
		addss	xmm5, xmm6

		addss	xmm3, xmm5

		movss[eax + 4], xmm3

		/////////////////////////////

		movss	xmm3, [ecx + 8]
		movss	xmm4, [ecx + 18h]
		movss	xmm5, [ecx + 28h]
		movss	xmm6, [ecx + 38h]

		mulss	xmm3, xmm0
		mulss	xmm4, xmm1
		mulss	xmm5, xmm2

		addss	xmm3, xmm4
		addss	xmm5, xmm6

		addss	xmm3, xmm5

		movss[eax + 8], xmm3

		pop	ebx
		pop	ecx
	}


	// 下面是根据VS2013编译的汇编指令翻译出来的SIMD代码
	/*__m128 xmm0;
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

	return pOutVector;*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SimdMatrix* PackedSimdMatrixMultiply(SimdMatrix* pOut, const SimdMatrix* pM1, const SimdMatrix* pM2)
{
	_asm
	{
		push	ebx
		push	ecx

		// 这3个mov指令占用了整个函数30%左右的CPU时间
		mov		eax, pOut
		mov		ebx, pM1
		mov		ecx, pM2

		movaps	xmm0, [ecx]
		movaps	xmm1, [ecx + 10h]
		movaps	xmm2, [ecx + 20h]
		movaps	xmm3, [ecx + 30h]

		/////////////////////////////

		movss	xmm4, [ebx]
		movss	xmm5, [ebx + 4]
		movss	xmm6, [ebx + 8]
		movss	xmm7, [ebx + 0Ch]

		// shufps指令用于将xmm寄存器的第一个float填充到剩余的3个float，即4个float都等于第一个float
		shufps	xmm4, xmm4, 0
		shufps	xmm5, xmm5, 0
		shufps	xmm6, xmm6, 0
		shufps	xmm7, xmm7, 0

		mulps	xmm4, xmm0
		mulps	xmm5, xmm1
		mulps	xmm6, xmm2
		mulps	xmm7, xmm3

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		addps	xmm4, xmm6
		movaps [eax], xmm4

		/////////////////////////////

		movss	xmm4, [ebx + 10h]
		movss	xmm5, [ebx + 14h]
		movss	xmm6, [ebx + 18h]
		movss	xmm7, [ebx + 1Ch]

		shufps	xmm4, xmm4, 0
		shufps	xmm5, xmm5, 0
		shufps	xmm6, xmm6, 0
		shufps	xmm7, xmm7, 0

		mulps	xmm4, xmm0
		mulps	xmm5, xmm1
		mulps	xmm6, xmm2
		mulps	xmm7, xmm3

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		addps	xmm4, xmm6
		movaps [eax + 10h], xmm4

		/////////////////////////////

		movss	xmm4, [ebx + 20h]
		movss	xmm5, [ebx + 24h]
		movss	xmm6, [ebx + 28h]
		movss	xmm7, [ebx + 2Ch]

		shufps	xmm4, xmm4, 0
		shufps	xmm5, xmm5, 0
		shufps	xmm6, xmm6, 0
		shufps	xmm7, xmm7, 0

		mulps	xmm4, xmm0
		mulps	xmm5, xmm1
		mulps	xmm6, xmm2
		mulps	xmm7, xmm3

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		addps	xmm4, xmm6
		movaps [eax + 20h], xmm4

		/////////////////////////////

		movss	xmm4, [ebx + 30h]
		movss	xmm5, [ebx + 34h]
		movss	xmm6, [ebx + 38h]
		movss	xmm7, [ebx + 3Ch]

		shufps	xmm4, xmm4, 0
		shufps	xmm5, xmm5, 0
		shufps	xmm6, xmm6, 0
		shufps	xmm7, xmm7, 0

		mulps	xmm4, xmm0
		mulps	xmm5, xmm1
		mulps	xmm6, xmm2
		mulps	xmm7, xmm3

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		addps	xmm4, xmm6
		movaps [eax + 30h], xmm4

		/////////////////////////////

		pop	ebx
		pop	ecx
	}


	/*const __m128* pM1Row = reinterpret_cast<const __m128*>(pM1);
	const __m128* pM2Row = reinterpret_cast<const __m128*>(pM2);
	__m128* pOutRow = reinterpret_cast<__m128*>(pOut);

	__m128 tempV4, R0, R1, R2, R3;

	tempV4 = _mm_mul_ps(_mm_shuffle_ps(pM1Row[0], pM1Row[0], ShuffleMask0), pM2Row[0]);
	tempV4 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(pM1Row[0], pM1Row[0], ShuffleMask1), pM2Row[1]), tempV4);
	tempV4 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(pM1Row[0], pM1Row[0], ShuffleMask2), pM2Row[2]), tempV4);
	R0 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(pM1Row[0], pM1Row[0], ShuffleMask3), pM2Row[3]), tempV4);

	tempV4 =				_mm_mul_ps(	_mm_shuffle_ps(pM1Row[1], pM1Row[1], ShuffleMask0), pM2Row[0]);
	tempV4 =	_mm_add_ps(	_mm_mul_ps(	_mm_shuffle_ps(pM1Row[1], pM1Row[1], ShuffleMask1), pM2Row[1]), tempV4);
	tempV4 =	_mm_add_ps(	_mm_mul_ps(	_mm_shuffle_ps(pM1Row[1], pM1Row[1], ShuffleMask2), pM2Row[2]), tempV4);
	R1 =		_mm_add_ps(	_mm_mul_ps(	_mm_shuffle_ps(pM1Row[1], pM1Row[1], ShuffleMask3), pM2Row[3]), tempV4); 
	
	tempV4 = _mm_mul_ps(_mm_shuffle_ps(pM1Row[2], pM1Row[2], ShuffleMask0), pM2Row[0]);
	tempV4 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(pM1Row[2], pM1Row[2], ShuffleMask1), pM2Row[1]), tempV4);
	tempV4 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(pM1Row[2], pM1Row[2], ShuffleMask2), pM2Row[2]), tempV4);
	R2 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(pM1Row[2], pM1Row[2], ShuffleMask3), pM2Row[3]), tempV4);

	tempV4 = _mm_mul_ps(_mm_shuffle_ps(pM1Row[3], pM1Row[3], ShuffleMask0), pM2Row[0]);
	tempV4 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(pM1Row[3], pM1Row[3], ShuffleMask1), pM2Row[1]), tempV4);
	tempV4 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(pM1Row[3], pM1Row[3], ShuffleMask2), pM2Row[2]), tempV4);
	R3 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(pM1Row[3], pM1Row[3], ShuffleMask3), pM2Row[3]), tempV4);

	pOutRow[0] = R0;
	pOutRow[1] = R1;
	pOutRow[2] = R2;
	pOutRow[3] = R3;

	return pOut;*/
}

D3DXMATRIX* SimdMatrixMultiply(D3DXMATRIX* pOut, const D3DXMATRIX* pM1, const D3DXMATRIX* pM2)
{
	__m128 Matrix2Row1 = _mm_loadu_ps(pM2->m[0]);
	__m128 Matrix2Row2 = _mm_loadu_ps(pM2->m[1]);
	__m128 Matrix2Row3 = _mm_loadu_ps(pM2->m[2]);
	__m128 Matrix2Row4 = _mm_loadu_ps(pM2->m[3]);

	// _mm_set1_ps(float f)，令向量的四个分量都等于f
	__m128 Matrix1Row1 = _mm_set1_ps(pM1->_11);
	__m128 Matrix1Row2 = _mm_set1_ps(pM1->_12);
	__m128 Matrix1Row3 = _mm_set1_ps(pM1->_13);
	__m128 Matrix1Row4 = _mm_set1_ps(pM1->_14);

	Matrix1Row1 = _mm_mul_ps(Matrix1Row1, Matrix2Row1);
	Matrix1Row2 = _mm_mul_ps(Matrix1Row2, Matrix2Row2);
	Matrix1Row3 = _mm_mul_ps(Matrix1Row3, Matrix2Row3);
	Matrix1Row4 = _mm_mul_ps(Matrix1Row4, Matrix2Row4);

	Matrix1Row1 = _mm_add_ps(Matrix1Row1, Matrix1Row2);
	Matrix1Row3 = _mm_add_ps(Matrix1Row3, Matrix1Row4);

	Matrix1Row1 = _mm_add_ps(Matrix1Row1, Matrix1Row3);
	_mm_storeu_ps(pOut->m[0], Matrix1Row1);

	Matrix1Row1 = _mm_set1_ps(pM1->_21);
	Matrix1Row2 = _mm_set1_ps(pM1->_22);
	Matrix1Row3 = _mm_set1_ps(pM1->_23);
	Matrix1Row4 = _mm_set1_ps(pM1->_24);

	Matrix1Row1 = _mm_mul_ps(Matrix1Row1, Matrix2Row1);
	Matrix1Row2 = _mm_mul_ps(Matrix1Row2, Matrix2Row2);
	Matrix1Row3 = _mm_mul_ps(Matrix1Row3, Matrix2Row3);
	Matrix1Row4 = _mm_mul_ps(Matrix1Row4, Matrix2Row4);

	Matrix1Row1 = _mm_add_ps(Matrix1Row1, Matrix1Row2);
	Matrix1Row3 = _mm_add_ps(Matrix1Row3, Matrix1Row4);

	Matrix1Row1 = _mm_add_ps(Matrix1Row1, Matrix1Row3);
	_mm_storeu_ps(pOut->m[1], Matrix1Row1);

	Matrix1Row1 = _mm_set1_ps(pM1->_31);
	Matrix1Row2 = _mm_set1_ps(pM1->_32);
	Matrix1Row3 = _mm_set1_ps(pM1->_33);
	Matrix1Row4 = _mm_set1_ps(pM1->_34);

	Matrix1Row1 = _mm_mul_ps(Matrix1Row1, Matrix2Row1);
	Matrix1Row2 = _mm_mul_ps(Matrix1Row2, Matrix2Row2);
	Matrix1Row3 = _mm_mul_ps(Matrix1Row3, Matrix2Row3);
	Matrix1Row4 = _mm_mul_ps(Matrix1Row4, Matrix2Row4);

	Matrix1Row1 = _mm_add_ps(Matrix1Row1, Matrix1Row2);
	Matrix1Row3 = _mm_add_ps(Matrix1Row3, Matrix1Row4);

	Matrix1Row1 = _mm_add_ps(Matrix1Row1, Matrix1Row3);
	_mm_storeu_ps(pOut->m[2], Matrix1Row1);

	Matrix1Row1 = _mm_set1_ps(pM1->_41);
	Matrix1Row2 = _mm_set1_ps(pM1->_42);
	Matrix1Row3 = _mm_set1_ps(pM1->_43);
	Matrix1Row4 = _mm_set1_ps(pM1->_44);

	Matrix1Row1 = _mm_mul_ps(Matrix1Row1, Matrix2Row1);
	Matrix1Row2 = _mm_mul_ps(Matrix1Row2, Matrix2Row2);
	Matrix1Row3 = _mm_mul_ps(Matrix1Row3, Matrix2Row3);
	Matrix1Row4 = _mm_mul_ps(Matrix1Row4, Matrix2Row4);

	Matrix1Row1 = _mm_add_ps(Matrix1Row1, Matrix1Row2);
	Matrix1Row3 = _mm_add_ps(Matrix1Row3, Matrix1Row4);

	Matrix1Row1 = _mm_add_ps(Matrix1Row1, Matrix1Row3);
	_mm_storeu_ps(pOut->m[3], Matrix1Row1);

	return pOut;
}

D3DXMATRIX* AsmMatrixMultiply(D3DXMATRIX* pOut, const D3DXMATRIX* pM1, const D3DXMATRIX* pM2)
{
	_asm
	{
		push	ebx
		push	ecx

		// 这3个mov指令占用了整个函数30%左右的CPU时间
		mov		eax, pOut
		mov		ebx, pM1
		mov		ecx, pM2

		movups	xmm0, [ecx]
		movups	xmm1, [ecx + 10h]
		movups	xmm2, [ecx + 20h]
		movups	xmm3, [ecx + 30h]

		/////////////////////////////

		movss	xmm4, [ebx]
		movss	xmm5, [ebx + 4]
		movss	xmm6, [ebx + 8]
		movss	xmm7, [ebx + 0Ch]

		// shufps指令用于将xmm寄存器的第一个float填充到剩余的3个float，即4个float都等于第一个float
		shufps	xmm4, xmm4, 0
		shufps	xmm5, xmm5, 0
		shufps	xmm6, xmm6, 0
		shufps	xmm7, xmm7, 0

		mulps	xmm4, xmm0
		mulps	xmm5, xmm1
		mulps	xmm6, xmm2
		mulps	xmm7, xmm3

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		addps	xmm4, xmm6
		movups[eax], xmm4

		/////////////////////////////

		movss	xmm4, [ebx + 10h]
		movss	xmm5, [ebx + 14h]
		movss	xmm6, [ebx + 18h]
		movss	xmm7, [ebx + 1Ch]

		shufps	xmm4, xmm4, 0
		shufps	xmm5, xmm5, 0
		shufps	xmm6, xmm6, 0
		shufps	xmm7, xmm7, 0

		mulps	xmm4, xmm0
		mulps	xmm5, xmm1
		mulps	xmm6, xmm2
		mulps	xmm7, xmm3

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		addps	xmm4, xmm6
		movups[eax + 10h], xmm4

		/////////////////////////////

		movss	xmm4, [ebx + 20h]
		movss	xmm5, [ebx + 24h]
		movss	xmm6, [ebx + 28h]
		movss	xmm7, [ebx + 2Ch]

		shufps	xmm4, xmm4, 0
		shufps	xmm5, xmm5, 0
		shufps	xmm6, xmm6, 0
		shufps	xmm7, xmm7, 0

		mulps	xmm4, xmm0
		mulps	xmm5, xmm1
		mulps	xmm6, xmm2
		mulps	xmm7, xmm3

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		addps	xmm4, xmm6
		movups[eax + 20h], xmm4

		/////////////////////////////

		movss	xmm4, [ebx + 30h]
		movss	xmm5, [ebx + 34h]
		movss	xmm6, [ebx + 38h]
		movss	xmm7, [ebx + 3Ch]

		shufps	xmm4, xmm4, 0
		shufps	xmm5, xmm5, 0
		shufps	xmm6, xmm6, 0
		shufps	xmm7, xmm7, 0

		mulps	xmm4, xmm0
		mulps	xmm5, xmm1
		mulps	xmm6, xmm2
		mulps	xmm7, xmm3

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		addps	xmm4, xmm6
		movups[eax + 30h], xmm4

		/////////////////////////////

		pop	ebx
		pop	ecx
	}
}