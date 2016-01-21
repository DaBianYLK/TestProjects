#ifdef _DEBUG
#pragma comment(lib, "TestUtilsd.lib")
#pragma comment(lib, "MyDXFunctionLibraryd.lib")
#else
#pragma comment(lib, "TestUtils.lib")
#pragma comment(lib, "MyDXFunctionLibrary.lib")
#endif

#include "../TestUtils/DB_Log.h"
#include "../TestUtils/DB_Timer.h"
#include "../MyDXFctionLibrary/MyDXFuntion.h"

#include <iostream>
#include <d3dx9.h>

using namespace std;
using namespace DaBianYLK;

/*
* 测试目的：比较常用DX函数与自己实现函数的效率

在测试中，矩阵内存是否对齐16字节对效率几乎没有影响
*/

const unsigned uCalculationCount = 2000000;
const bool g_bCompareQuaternion = true;
const bool g_bCompareVectorMatrix = true;
const bool g_bUseParallelVectorCompute = true;
const bool g_bCompareMatrixMatrix = true;

//////////////////////////////////////////////////// Quaternion ///////////////////////////////////////////////////////////////

D3DXVECTOR3* g_pAxis;
float* g_pAngle;
D3DXQUATERNION* g_pQuaternion;

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

//////////////////////////////////////////////////// Vector * Matrix ///////////////////////////////////////////////////////////////
D3DXMATRIX* g_pMatrix;
D3DXVECTOR3* g_pPosition;
SimdMatrix* g_pPackedMatrix;
SimdVector3* g_pPackedPosition;

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

void PackedAsmVectorTransformCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		PackedAsmVectorTransform(g_pPackedPosition, g_pPackedPosition, g_pPackedMatrix);
	}
}

void PackedSimdVectorTransformCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		PackedSimdVectorTransform(g_pPackedPosition, g_pPackedPosition, g_pPackedMatrix);
	}
}

void SimdVectorTransformCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		if (g_bUseParallelVectorCompute)
		{
			SimdVectorTransformParallel(g_pPosition, g_pPosition, g_pMatrix);
		}
		else
		{
			SimdVectorTransformUnparallel(g_pPosition, g_pPosition, g_pMatrix);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////// Matrix * Matrix ///////////////////////////////////////////////////////////////
D3DXMATRIX* g_pMatrix1;
D3DXMATRIX* g_pMatrix2;
SimdMatrix* g_pPackedMatrix1;
SimdMatrix* g_pPackedMatrix2;

void DXMatrixMultiplyCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		D3DXMatrixMultiply(g_pMatrix1, g_pMatrix1, g_pMatrix2);
	}
}

void PackedSimdMatrixMultiplyCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		PackedSimdMatrixMultiply(g_pPackedMatrix1, g_pPackedMatrix1, g_pPackedMatrix2);
	}
}

void SimdMatrixMultiplyCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		SimdMatrixMultiply(g_pMatrix1, g_pMatrix1, g_pMatrix2);
	}
}

void AsmMatrixMultiplyCompute()
{
	for (unsigned i = 0; i < uCalculationCount; ++i)
	{
		AsmMatrixMultiply(g_pMatrix1, g_pMatrix1, g_pMatrix2);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	system("pause");

	if (g_bCompareQuaternion)
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

		cout << endl;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (g_bCompareVectorMatrix)
	{
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
		g_pPackedPosition = new SimdVector3();
		g_pPackedMatrix = new SimdMatrix();

		g_pPackedPosition->x = g_pPosition->x;
		g_pPackedPosition->y = g_pPosition->y;
		g_pPackedPosition->z = g_pPosition->z;
		for (int r = 0; r < 4; ++r)
		{
			for (int c = 0; c < 4; ++c)
			{
				g_pPackedMatrix->m[r][c] = g_pMatrix->m[r][c];
			}
		}

		BENCHMARK(PackedSimdVectorTransformCompute, PackedSimdVectorTransformCompute());
		Log("Result: (%f, %f, %f)", g_pPackedPosition->x, g_pPackedPosition->y, g_pPackedPosition->z); // 这里要输出一次result,否则编译器的优化会删除执行运算的代码

		delete g_pMatrix;
		delete g_pPosition;
		delete g_pPackedMatrix;
		delete g_pPackedPosition;


		g_pMatrix = new D3DXMATRIX();
		D3DXMatrixLookAtLH(g_pMatrix, new D3DXVECTOR3(33.0f, 33.0f, 33.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 10.0f, 0.0f));
		g_pPosition = new D3DXVECTOR3(5.0f, 5.0f, 5.0f);

		BENCHMARK(SimdVectorTransformCompute, SimdVectorTransformCompute());
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
		g_pPackedPosition = new SimdVector3();
		g_pPackedMatrix = new SimdMatrix();

		g_pPackedPosition->x = g_pPosition->x;
		g_pPackedPosition->y = g_pPosition->y;
		g_pPackedPosition->z = g_pPosition->z;
		for (int r = 0; r < 4; ++r)
		{
			for (int c = 0; c < 4; ++c)
			{
				g_pPackedMatrix->m[r][c] = g_pMatrix->m[r][c];
			}
		}

		BENCHMARK(PackedAsmVectorTransformCompute, PackedAsmVectorTransformCompute());
		Log("Result: (%f, %f, %f)", g_pPackedPosition->x, g_pPackedPosition->y, g_pPackedPosition->z); // 这里要输出一次result,否则编译器的优化会删除执行运算的代码

		delete g_pMatrix;
		delete g_pPosition;
		delete g_pPackedMatrix;
		delete g_pPackedPosition;

		cout << endl;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (g_bCompareMatrixMatrix)
	{
		g_pMatrix1 = new D3DXMATRIX();
		g_pMatrix2 = new D3DXMATRIX();
		D3DXMatrixIdentity(g_pMatrix1);
		D3DXMatrixLookAtLH(g_pMatrix2, new D3DXVECTOR3(3.0f, 3.0f, 3.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 1.0f, 0.0f));

		BENCHMARK(DXMatrixMultiplyCompute, DXMatrixMultiplyCompute());
		// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_11, g_pMatrix1->_12, g_pMatrix1->_13, g_pMatrix1->_14);
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_21, g_pMatrix1->_22, g_pMatrix1->_23, g_pMatrix1->_24);
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_31, g_pMatrix1->_32, g_pMatrix1->_33, g_pMatrix1->_34);
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_41, g_pMatrix1->_42, g_pMatrix1->_43, g_pMatrix1->_44);

		delete g_pMatrix1;
		delete g_pMatrix2;

		g_pMatrix1 = new D3DXMATRIX();
		g_pMatrix2 = new D3DXMATRIX();
		D3DXMatrixIdentity(g_pMatrix1);
		D3DXMatrixLookAtLH(g_pMatrix2, new D3DXVECTOR3(3.0f, 3.0f, 3.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 1.0f, 0.0f));

		g_pPackedMatrix1 = new SimdMatrix();
		g_pPackedMatrix2 = new SimdMatrix();

		for (int r = 0; r < 4; ++r)
		{
			for (int c = 0; c < 4; ++c)
			{
				g_pPackedMatrix1->m[r][c] = g_pMatrix1->m[r][c];
				g_pPackedMatrix2->m[r][c] = g_pMatrix2->m[r][c];
			}
		}

		BENCHMARK(PackedSimdMatrixMultiplyCompute, PackedSimdMatrixMultiplyCompute());
		// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
		Log("Result: (%f, %f, %f, %f)", g_pPackedMatrix1->_11, g_pPackedMatrix1->_12, g_pPackedMatrix1->_13, g_pPackedMatrix1->_14);
		Log("Result: (%f, %f, %f, %f)", g_pPackedMatrix1->_21, g_pPackedMatrix1->_22, g_pPackedMatrix1->_23, g_pPackedMatrix1->_24);
		Log("Result: (%f, %f, %f, %f)", g_pPackedMatrix1->_31, g_pPackedMatrix1->_32, g_pPackedMatrix1->_33, g_pPackedMatrix1->_34);
		Log("Result: (%f, %f, %f, %f)", g_pPackedMatrix1->_41, g_pPackedMatrix1->_42, g_pPackedMatrix1->_43, g_pPackedMatrix1->_44);

		delete g_pMatrix1;
		delete g_pMatrix2;
		delete g_pPackedMatrix1;
		delete g_pPackedMatrix2;

		g_pMatrix1 = new D3DXMATRIX();
		g_pMatrix2 = new D3DXMATRIX(); 
		D3DXMatrixIdentity(g_pMatrix1);
		D3DXMatrixLookAtLH(g_pMatrix2, new D3DXVECTOR3(3.0f, 3.0f, 3.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		
		BENCHMARK(SimdMatrixMultiplyCompute, SimdMatrixMultiplyCompute());
		// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_11, g_pMatrix1->_12, g_pMatrix1->_13, g_pMatrix1->_14);
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_21, g_pMatrix1->_22, g_pMatrix1->_23, g_pMatrix1->_24);
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_31, g_pMatrix1->_32, g_pMatrix1->_33, g_pMatrix1->_34);
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_41, g_pMatrix1->_42, g_pMatrix1->_43, g_pMatrix1->_44);

		delete g_pMatrix1;
		delete g_pMatrix2;

		g_pMatrix1 = new D3DXMATRIX();
		g_pMatrix2 = new D3DXMATRIX();
		D3DXMatrixIdentity(g_pMatrix1);
		D3DXMatrixLookAtLH(g_pMatrix2, new D3DXVECTOR3(3.0f, 3.0f, 3.0f), new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 1.0f, 0.0f));

		BENCHMARK(AsmMatrixMultiplyCompute, AsmMatrixMultiplyCompute());
		// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_11, g_pMatrix1->_12, g_pMatrix1->_13, g_pMatrix1->_14);
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_21, g_pMatrix1->_22, g_pMatrix1->_23, g_pMatrix1->_24);
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_31, g_pMatrix1->_32, g_pMatrix1->_33, g_pMatrix1->_34);
		Log("Result: (%f, %f, %f, %f)", g_pMatrix1->_41, g_pMatrix1->_42, g_pMatrix1->_43, g_pMatrix1->_44);

		delete g_pMatrix1;
		delete g_pMatrix2;

		cout << endl;
	}
	
	system("pause");

	return 0;
}
