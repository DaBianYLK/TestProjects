#include <Windows.h>

namespace DaBianYLK {
	static LARGE_INTEGER nFrequency;		// 系统时钟频率
	static LARGE_INTEGER nStart;
	static LARGE_INTEGER nCurrent;

	void StartTimer();
	float GetPastSeconds();
}