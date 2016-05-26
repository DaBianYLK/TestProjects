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

const unsigned int u32CalculationTimes = 50000000;
unsigned char* aryExponents = new unsigned char[u32CalculationTimes];
unsigned char u8Result = 0;

void InitExponents()
{
	for (unsigned int i = 0; i < u32CalculationTimes; ++i)
	{
		aryExponents[i] = i % 8;
	}
}

void PowWithDoubleForShiftBit()
{
	for (unsigned int i = 0; i < u32CalculationTimes; ++i)
	{
		u8Result = static_cast<unsigned char>(pow(2.0, static_cast<double>(aryExponents[i])));
	}
}

void ShiftBit()
{
	for (unsigned int i = 0; i < u32CalculationTimes; ++i)
	{
		u8Result = 1 << aryExponents[i];
	}
}

void PowWithDoubleForSquare()
{
	for (unsigned int i = 0; i < u32CalculationTimes; ++i)
	{
		u8Result = static_cast<unsigned char>(pow(static_cast<double>(aryExponents[i]), 2.0));
	}
}

void Square()
{
	for (unsigned int i = 0; i < u32CalculationTimes; ++i)
	{
		u8Result = aryExponents[i] * aryExponents[i];
	}
}

void PowWithDoubleForMod()
{
	for (unsigned int i = 0; i < u32CalculationTimes; ++i)
	{
		u8Result = static_cast<unsigned char>(pow(-1.0, static_cast<double>(aryExponents[i])));
	}
}

void Mod()
{
	for (unsigned int i = 0; i < u32CalculationTimes; ++i)
	{
		u8Result = (aryExponents[i] % 2) ? -1 : 1;
	}
}

void And()
{
	for (unsigned int i = 0; i < u32CalculationTimes; ++i)
	{
		u8Result = (aryExponents[i] & 1) ? -1 : 1;		// Release模式下，取余运算效率高于位运算（效率约差1倍）
	}
}

int main()
{
	BENCHMARK(PowWithDoubleForShiftBit, PowWithDoubleForShiftBit());
	cout << u8Result << endl;

	BENCHMARK(ShiftBit, ShiftBit());
	cout << u8Result << endl;

	BENCHMARK(PowWithDoubleForSquare, PowWithDoubleForSquare());
	cout << u8Result << endl;

	BENCHMARK(Square, Square());
	cout << u8Result << endl;

	BENCHMARK(PowWithDoubleForMod, PowWithDoubleForMod());
	cout << u8Result << endl;

	BENCHMARK(Mod, Mod());
	cout << u8Result << endl;

	BENCHMARK(And, And());
	cout << u8Result << endl;

	system("pause");
	return 0;
}