#pragma once

#include <d3dx9.h>
#include <xmmintrin.h>

static const unsigned int ShuffleMask0 = 0;
static const unsigned int ShuffleMask1 = (1 | (1 << 2) | (1 << 4) | (1 << 6));
static const unsigned int ShuffleMask2 = (2 | (2 << 2) | (2 << 4) | (2 << 6));
static const unsigned int ShuffleMask3 = (3 | (3 << 2) | (3 << 4) | (3 << 6));

struct SimdVector3
{
	union
	{
		__m128 float4;

		struct
		{
			float x;
			float y;
			float z;
			float deprecation;
		};
	};

	void* operator new(size_t size)
	{
		return _aligned_malloc(size, 16);
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}
};

struct SimdMatrix
{
	union
	{
		struct
		{
			__declspec(align(16)) float m[4][4];
		};

		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
	};

	void* operator new(size_t size)
	{
		return _aligned_malloc(size, 16);
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}
};

D3DXQUATERNION* AxisAngleToQuaternion(D3DXQUATERNION* pQuaternion, const D3DXVECTOR3* pAxis, float angle);
void QuaternionToAxisAngle(const D3DXQUATERNION* pQuaternion, D3DXVECTOR3* pAxis, float* pAngle);

D3DXVECTOR3* VectorTransform(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix);
D3DXVECTOR3* SimdVectorTransformParallel(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix);
D3DXVECTOR3* SimdVectorTransformUnparallel(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix);
SimdVector3* PackedSimdVectorTransform(SimdVector3* pOut, const SimdVector3* pIn, const SimdMatrix* pM);
D3DXVECTOR3* AsmVectorTransform(D3DXVECTOR3* pOutVector, const D3DXVECTOR3* pInVector, const D3DXMATRIX* pMatrix);
SimdVector3* PackedAsmVectorTransform(SimdVector3* pOut, const SimdVector3* pIn, const SimdMatrix* pM);

SimdMatrix* PackedSimdMatrixMultiply(SimdMatrix* pOut, const SimdMatrix* pM1, const SimdMatrix* pM2);
D3DXMATRIX* SimdMatrixMultiply(D3DXMATRIX* pOut, const D3DXMATRIX* pM1, const D3DXMATRIX* pM2);
D3DXMATRIX* AsmMatrixMultiply(D3DXMATRIX* pOut, const D3DXMATRIX* pM1, const D3DXMATRIX* pM2);