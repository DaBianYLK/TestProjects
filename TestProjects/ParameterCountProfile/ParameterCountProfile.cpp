/*--------------------------------------------------------------------------------------------------------------------*\
   【CREATE】	
	AUTH :	大便一箩筐																			   DATE : 2016-06-06
	DESC :	比较函数参数个数对性能的影响，具体比较用例如下：
			用例一	不存在函数嵌套的情况下，传递10个int 参数与传递1 个参数（通过地址取全局变量）
			用例二	存在参数复用的情况下，传递10个int 参数与传递1 个参数比较性能（参数复用10次）
			用例三	存参数多次传递的情况下，传递10个int 参数与传递1 个参数比较性能（参数传递10次）

	结果 :	用例一	禁用内联函数优化的情况下，循环500万次，传递10个参数比传递一个参数多用时0.006s
			用例二  禁用内联函数优化的情况下，循环500万次，传递10个参数比传递一个参数少用时0.056s / 10 = 0.006s
			用例三  禁用内联函数优化的情况下，循环500万次，传递10个参数比传递一个参数多用时0.074s / 10 = 0.007s

	预计 ：	若使用3.30GHz主频的CPU，每减少2.5亿个int型参数的传递可以节约0.03s
\*--------------------------------------------------------------------------------------------------------------------*/

#ifdef _DEBUG
#pragma comment(lib, "../Debug/TestUtilsd.lib")
#else
#pragma comment(lib, "../Release/TestUtils.lib")
#endif

#include "../TestUtils/DB_Log.h"
#include "../TestUtils/DB_Timer.h"

#include <iostream>

using namespace std;
using namespace DaBianYLK;

const unsigned int g_u32LoopCount = 5000000;
const unsigned int g_u32NestedCount = 10;
int		           g_i32Result = 0;

struct Param
{
	int aryNumbers[10];
};

Param*	g_AryParams = new Param[g_u32LoopCount];

int Add(int* aryIntegers)
{
	int i32Result = aryIntegers[0];
	for (int i = 1; i < 10; ++i)
	{
		i32Result += aryIntegers[i];
	}

	return i32Result;
}

int Add0(int* aryIntegers)
{
	return Add(aryIntegers);
}

int Add1(int* aryIntegers)
{
	return Add0(aryIntegers);
}

int Add2(int* aryIntegers)
{
	return Add1(aryIntegers);
}

int Add3(int* aryIntegers)
{
	return Add2(aryIntegers);
}

int Add4(int* aryIntegers)
{
	return Add3(aryIntegers);
}

int Add5(int* aryIntegers)
{
	return Add4(aryIntegers);
}

int Add6(int* aryIntegers)
{
	return Add5(aryIntegers);
}

int Add7(int* aryIntegers)
{
	return Add6(aryIntegers);
}

int Add8(int* aryIntegers)
{
	return Add7(aryIntegers);
}

int Add9(int* aryIntegers)
{
	return Add0(aryIntegers);
}

int Add10(int* aryIntegers)
{
	return Add9(aryIntegers);
}

int Add(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	int i32Result = i32A;
	i32Result += i32B;
	i32Result += i32C;
	i32Result += i32D;
	i32Result += i32E;
	i32Result += i32F;
	i32Result += i32G;
	i32Result += i32H;
	i32Result += i32I;
	i32Result += i32J;

	return i32Result;
}

int Add0(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	return Add(i32A, i32B, i32C, i32D, i32E, i32F, i32G, i32H, i32I, i32J);
}

int Add1(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	return Add0(i32A, i32B, i32C, i32D, i32E, i32F, i32G, i32H, i32I, i32J);
}

int Add2(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	return Add1(i32A, i32B, i32C, i32D, i32E, i32F, i32G, i32H, i32I, i32J);
}

int Add3(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	return Add2(i32A, i32B, i32C, i32D, i32E, i32F, i32G, i32H, i32I, i32J);
}

int Add4(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	return Add3(i32A, i32B, i32C, i32D, i32E, i32F, i32G, i32H, i32I, i32J);
}

int Add5(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	return Add4(i32A, i32B, i32C, i32D, i32E, i32F, i32G, i32H, i32I, i32J);
}

int Add6(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	return Add5(i32A, i32B, i32C, i32D, i32E, i32F, i32G, i32H, i32I, i32J);
}

int Add7(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	return Add6(i32A, i32B, i32C, i32D, i32E, i32F, i32G, i32H, i32I, i32J);
}

int Add8(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	return Add7(i32A, i32B, i32C, i32D, i32E, i32F, i32G, i32H, i32I, i32J);
}

int Add9(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	return Add8(i32A, i32B, i32C, i32D, i32E, i32F, i32G, i32H, i32I, i32J);
}

int Add10(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	return Add9(i32A, i32B, i32C, i32D, i32E, i32F, i32G, i32H, i32I, i32J);
}

void NestedAdd(int* aryIntegers)
{
	for (unsigned int i = 0; i < g_u32NestedCount; ++i)
	{
		g_i32Result += Add(aryIntegers);
	}
}

void NestedAdd(int i32A, int i32B, int i32C, int i32D, int i32E, int i32F, int i32G, int i32H, int i32I, int i32J)
{
	for (unsigned int i = 0; i < g_u32NestedCount; ++i)
	{
		g_i32Result += Add(i32A,  i32B,  i32C,  i32D,  i32E,  i32F,  i32G,  i32H,  i32I,  i32J);
	}
}

void NoNestAddWithOneParam()
{
	for (unsigned int i = 0; i < g_u32LoopCount; ++i)
	{
		g_i32Result += Add(reinterpret_cast<int*>(&g_AryParams[i]));
	}
}

void NoNestAddWithTenParam()
{
	for (unsigned int i = 0; i < g_u32LoopCount; ++i)
	{
		g_i32Result += Add(
			g_AryParams[i].aryNumbers[0],
			g_AryParams[i].aryNumbers[1],
			g_AryParams[i].aryNumbers[2],
			g_AryParams[i].aryNumbers[3],
			g_AryParams[i].aryNumbers[4],
			g_AryParams[i].aryNumbers[5],
			g_AryParams[i].aryNumbers[6],
			g_AryParams[i].aryNumbers[7],
			g_AryParams[i].aryNumbers[8],
			g_AryParams[i].aryNumbers[9]);
	}
}

void NestAddWithOneParamWide()
{
	for (unsigned int i = 0; i < g_u32LoopCount; ++i)
	{
		NestedAdd(reinterpret_cast<int*>(&g_AryParams[i]));
	}
}

void NestAddWithTenParamWide()
{
	for (unsigned int i = 0; i < g_u32LoopCount; ++i)
	{
		NestedAdd(
			g_AryParams[i].aryNumbers[0],
			g_AryParams[i].aryNumbers[1],
			g_AryParams[i].aryNumbers[2],
			g_AryParams[i].aryNumbers[3],
			g_AryParams[i].aryNumbers[4],
			g_AryParams[i].aryNumbers[5],
			g_AryParams[i].aryNumbers[6],
			g_AryParams[i].aryNumbers[7],
			g_AryParams[i].aryNumbers[8],
			g_AryParams[i].aryNumbers[9]);
	}
}

void NestAddWithOneParamDeep()
{
	for (unsigned int i = 0; i < g_u32LoopCount; ++i)
	{
		g_i32Result += Add10(reinterpret_cast<int*>(&g_AryParams[i]));
	}
}

void NestAddWithTenParamDeep()
{
	for (unsigned int i = 0; i < g_u32LoopCount; ++i)
	{
		g_i32Result += Add10(
			g_AryParams[i].aryNumbers[0],
			g_AryParams[i].aryNumbers[1],
			g_AryParams[i].aryNumbers[2],
			g_AryParams[i].aryNumbers[3],
			g_AryParams[i].aryNumbers[4],
			g_AryParams[i].aryNumbers[5],
			g_AryParams[i].aryNumbers[6],
			g_AryParams[i].aryNumbers[7],
			g_AryParams[i].aryNumbers[8],
			g_AryParams[i].aryNumbers[9]);
	}
}

int main()
{
	BENCHMARK(NoNestAddWithOneParam1, NoNestAddWithOneParam());
	cout << g_i32Result << endl;

	// 上面的计算是为了防止缓存影响性能比较结果

	BENCHMARK(NoNestAddWithOneParam, NoNestAddWithOneParam());
	cout << g_i32Result << endl;

	BENCHMARK(NoNestAddWithTenParam, NoNestAddWithTenParam());
	cout << g_i32Result << endl;

	BENCHMARK(NestAddWithOneParamWide, NestAddWithOneParamWide());
	cout << g_i32Result << endl;

	BENCHMARK(NestAddWithTenParamWide, NestAddWithTenParamWide());
	cout << g_i32Result << endl;

	BENCHMARK(NestAddWithOneParamDeep, NestAddWithOneParamDeep());
	cout << g_i32Result << endl;

	BENCHMARK(NestAddWithTenParamDeep, NestAddWithTenParamDeep());
	cout << g_i32Result << endl;

	//system("pause");
	return 0;
}