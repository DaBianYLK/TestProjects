#ifdef _DEBUG
#pragma comment(lib, "TestUtilsd.lib")
#else
#pragma comment(lib, "TestUtils.lib")
#endif

#include "../TestUtils/DB_Log.h"
#include "../TestUtils/DB_Timer.h"

#include <iostream>

using namespace std;
using namespace DaBianYLK;

/*
* 测试目的：判断浮点数四则运算之间的效率差异
*
* 测试方法：运行四则运算各1亿次，比较消耗的时间
*
* Debug模式下Profile结果——每种运算函数的CPU占用百分比：
*		乘法 6.29%
*		除法 42.86%
*		加法 5.98%
*		减法 6.32%
*
* Release模式下计时结果：
*		乘法 0.147805s
*		除法 4.411531s
*		加法 0.088647s
*		减法 0.087332s
*
* 结果分析：
*		1. 浮点数乘法、加减法效率相近，除法效率最低
*		2. 在Release模式下，浮点数乘法效率与除法的时间消耗比约为 1 ： 30
*
* 结论：
*		在希望效率最大化的情况下，需要尽量避免浮点数除法运算：
*		1. 精度要求不高时可以改为整数除法运算或移位运算
*		2. 多次除以同一个变量时，先转化为倒数，再使用乘法
*/

const unsigned g_uCalculationCount = 400000000;

const unsigned g_uMid = g_uCalculationCount / 2;

unsigned g_uA = 0;
unsigned g_uB = 0;

inline void SmallZero(int bA, unsigned& iA, unsigned& iB)
{
	if (bA > 0)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}


inline void Bigger(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA > bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

inline void BiggerEqual(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA >= bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

inline void Smaller(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA < bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

inline void SmallerEqual(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA <= bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

inline void Equal(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA == bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

inline void UnEqual(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA != bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

void SmallZeroOperator()
{
	int j = 0;
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		SmallZero(j, g_uA, g_uB);
		++j;
	}
}

void BiggerOperatorTrue()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		Bigger(g_uMid, i, g_uA, g_uB);
	}
}

void BiggerOperatorFalse()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		Bigger(i, g_uMid, g_uA, g_uB);
	}
}

void BiggerEqualOperatorTrue()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		BiggerEqual(g_uMid, i, g_uA, g_uB);
	}
}

void BiggerEqualOperatorFalse()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		BiggerEqual(i, g_uMid, g_uA, g_uB);
	}
}

void SmallerOperatorTrue()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		Smaller(i, g_uMid, g_uA, g_uB);
	}
}

void SmallerOperatorFalse()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		Smaller(g_uMid, i, g_uA, g_uB);
	}
}

void SmallerEqualOperatorTrue()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		SmallerEqual(i, g_uMid, g_uA, g_uB);
	}
}

void SmallerEqualOperatorFalse()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		SmallerEqual(g_uMid, i, g_uA, g_uB);
	}
}

void EqualOperatorTrue()
{
	for (unsigned i = 0, j = g_uCalculationCount; i < g_uCalculationCount; ++i, --j)
	{
		Equal(i, j, g_uA, g_uB);
	}
}

void EqualOperatorFalse()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		Equal(i, g_uCalculationCount, g_uA, g_uB);
	}
}

void UnEqualOperatorTrue()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		UnEqual(i, g_uCalculationCount, g_uA, g_uB);
	}
}

void UnEqualOperatorFalse()
{
	int j = 0;
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		UnEqual(i, j, g_uA, g_uB);
		++j;
	}
}

unsigned main(void) 
{
	system("pause");

	BENCHMARK(SmallZeroOperator, SmallZeroOperator());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(BiggerOperatorTrue, BiggerOperatorTrue());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(BiggerOperatorFalse, BiggerOperatorFalse());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(BiggerEqualOperatorTrue, BiggerEqualOperatorTrue());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(BiggerEqualOperatorFalse, BiggerEqualOperatorFalse());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(SmallerOperatorTrue, SmallerOperatorTrue());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(SmallerOperatorFalse, SmallerOperatorFalse());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(SmallerEqualOperatorTrue, SmallerEqualOperatorTrue());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(SmallerEqualOperatorFalse, SmallerEqualOperatorFalse());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(EqualOperatorTrue, EqualOperatorTrue());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(EqualOperatorFalse, EqualOperatorFalse());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(UnEqualOperatorTrue, UnEqualOperatorTrue());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(UnEqualOperatorFalse, UnEqualOperatorFalse());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", g_uA, g_uB);

	system("pause");

	return 0;
}