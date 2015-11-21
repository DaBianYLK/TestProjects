#pragma comment(lib, "TestUtils.lib")

#include "../TestUtils/DB_Log.h"
#include "../TestUtils/DB_Timer.h"

#include <iostream>

using namespace std;
using namespace DaBianYLK;

#define FLOAT_TO_INT(f, i) _asm fld f _asm fistp i

float* fTable = new float[1024];
const float fDenominator = 3.3f;
float fNumber = 1.0f;
const unsigned int nCalculation = 50000000;

inline void SetupFloatTable() {
	for (unsigned i = 0; i < 1023; ++i) {
		fTable[i] = (i + 1 + 0.33f) / fDenominator;
	}

	fTable[1023] = 1.0f / fDenominator;
}

inline void DirectCast() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		int tempI = fDenominator * fNumber;

		fNumber = fTable[tempI];
	}
}

inline void SseAsmCast() {
	_asm movss		xmm0, fNumber

	for (unsigned int i = 0; i < nCalculation; ++i) {
		_asm {
			mulss		xmm0, fDenominator
			cvttss2si	eax, xmm0
			mov			ebx,fTable
			movss		xmm0,dword ptr [ebx+eax*4]
			movss		fNumber,xmm0
		}
	}
}

inline void NormalAsmCast() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		float fTemp = fDenominator * fNumber;
		int iTemp;

		_asm fld fTemp
		_asm fistp iTemp

		fNumber = fTable[iTemp];
	}
}

inline void StaticCast() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		int tempI = static_cast<int>(fDenominator * fNumber);

		fNumber = fTable[tempI];
	}
}

int main(void) {
	SetupFloatTable();

	// 直接转换
	fNumber = 1.0f;
	BENCHMARK(DirectCast, DirectCast());
	Log("FNumber : %f", fNumber);

	// Trick
	fNumber = 1.0f;
	BENCHMARK(SseAsmCast, SseAsmCast());
	Log("FNumber : %f", fNumber);

	// Trick
	fNumber = 1.0f;
	BENCHMARK(NormalAsmCast, NormalAsmCast());
	Log("FNumber : %f", fNumber);

	// 静态转换
	fNumber = 1.0f;
	BENCHMARK(StaticCast, StaticCast());
	Log("FNumber : %f", fNumber);			// 至少要输出一次fNumber,否则编译器的优化会删除执行运算的代码

	system("pause");

	return 0;
}