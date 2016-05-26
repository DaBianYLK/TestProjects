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

const unsigned g_uCalculationCount = 40000000;

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

unsigned long long* g_u64ArrayA = new unsigned long long[g_uCalculationCount];
unsigned long long* g_u64ArrayB = new unsigned long long[g_uCalculationCount];
unsigned int* g_u32ArrayA = new unsigned int[g_uCalculationCount];
unsigned int* g_u32ArrayB = new unsigned int[g_uCalculationCount];
unsigned short* g_u16ArrayA = new unsigned short[g_uCalculationCount];
unsigned short* g_u16ArrayB = new unsigned short[g_uCalculationCount];
unsigned char* g_u8ArrayA = new unsigned char[g_uCalculationCount];
unsigned char* g_u8ArrayB = new unsigned char[g_uCalculationCount];

unsigned int uABiggerCount = 0;
unsigned int uBBiggerCount = 0;

void Unsigned64Comparision()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		if (g_u64ArrayA[i] > g_u64ArrayB[i])
		{
			++uABiggerCount;
		}
		else
		{
			//++uBBiggerCount;
		}
	}
}

void Unsigned32Comparision()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		if (g_u32ArrayA[i] > g_u32ArrayB[i])
		{
			++uABiggerCount;
		}
		else
		{
			//++uBBiggerCount;
		}
	}
}

void Unsigned16Comparision()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		if (g_u16ArrayA[i] > g_u16ArrayB[i])
		{
			++uABiggerCount;
		}
		else
		{
			//++uBBiggerCount;
		}
	}
}

void Unsigned8Comparision()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		if (g_u8ArrayA[i] > g_u8ArrayB[i])
		{
			++uABiggerCount;
		}
		else
		{
			//++uBBiggerCount;
		}
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

	uABiggerCount = 0;
	uBBiggerCount = 0;

	BENCHMARK(Unsigned64Comparision, Unsigned64Comparision());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", uABiggerCount, uBBiggerCount);

	uABiggerCount = 0;
	uBBiggerCount = 0;

	BENCHMARK(Unsigned32Comparision, Unsigned32Comparision());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", uABiggerCount, uBBiggerCount);

	uABiggerCount = 0;
	uBBiggerCount = 0;

	BENCHMARK(Unsigned16Comparision, Unsigned16Comparision());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", uABiggerCount, uBBiggerCount);

	uABiggerCount = 0;
	uBBiggerCount = 0;

	BENCHMARK(Unsigned8Comparision, Unsigned8Comparision());
	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Result : %u, %u", uABiggerCount, uBBiggerCount);

	system("pause");

	return 0;
}