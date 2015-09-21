#include "DBTimer.h"

void DaBianYLK::StartTimer() {
	QueryPerformanceFrequency(&DaBianYLK::nFrequency);	// 获取系统时钟频率（每秒运行次数）
	QueryPerformanceCounter(&DaBianYLK::nStart);		// 获取计时开始时系统时钟计数
}

float DaBianYLK::GetPastSeconds() {
	QueryPerformanceCounter(&DaBianYLK::nCurrent);		// 获取当前系统时钟计数
	
	return (float)(nCurrent.QuadPart - nStart.QuadPart) / nFrequency.QuadPart;
}