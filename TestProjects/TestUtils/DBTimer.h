#include <Windows.h>

namespace DaBianYLK {
	static LARGE_INTEGER nFrequency;		// ϵͳʱ��Ƶ��
	static LARGE_INTEGER nStart;
	static LARGE_INTEGER nCurrent;

	void StartTimer();
	float GetPastSeconds();
}