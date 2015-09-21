#include "DBTimer.h"

void DaBianYLK::StartTimer() {
	QueryPerformanceFrequency(&DaBianYLK::nFrequency);	// ��ȡϵͳʱ��Ƶ�ʣ�ÿ�����д�����
	QueryPerformanceCounter(&DaBianYLK::nStart);		// ��ȡ��ʱ��ʼʱϵͳʱ�Ӽ���
}

float DaBianYLK::GetPastSeconds() {
	QueryPerformanceCounter(&DaBianYLK::nCurrent);		// ��ȡ��ǰϵͳʱ�Ӽ���
	
	return (float)(nCurrent.QuadPart - nStart.QuadPart) / nFrequency.QuadPart;
}