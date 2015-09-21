#pragma comment(lib, "TestUtils.lib")

#include "DBLog.h"
#include "DBTimer.h"

#include <iostream>

using namespace std;
using namespace DaBianYLK;

/* 
 * ����Ŀ�ģ��жϸ�������������֮���Ч�ʲ���
 *
 * ���Է������������������1�ڴΣ��Ƚ����ĵ�ʱ��
 *
 * Debugģʽ��Profile�������ÿ�����㺯����CPUռ�ðٷֱȣ�
 *		�˷� 6.29%
 *		���� 42.86%
 *		�ӷ� 5.98%
 *		���� 6.32%
 *
 * Releaseģʽ�¼�ʱ�����
 *		�˷� 0.147805s
 *		���� 4.411531s
 *		�ӷ� 0.088647s
 *		���� 0.087332s
 *
 * ���������
 *		1. �������˷����Ӽ���Ч�����������Ч�����
 *		2. ��Releaseģʽ�£��������˷�Ч���������Ч�ʱ�ԼΪ1��30
 */

inline float Mul(float a, float b) {
	return a * b;
}

inline float Div(float a, float b) {
	return a / b;
}

inline float Add(float a, float b) {
	return a + b;
}

inline float Sub(float a, float b) {
	return a - b;
}

int main(void) {
	const unsigned int nCalculation = 99999999;
	float pastTime = 0.0f;

	// ************************* �˷� *************************
	float a = 1.11112f;
	float result = 12345.0f;

	StartTimer();

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Mul(result, a);
	}

	pastTime = GetPastSeconds();
	Log("Past Time : %fs, Result : %f", pastTime, result);	// ����Ҫ���һ��result,������������Զ��Ż����޸�δʹ�ñ��������

	// ************************* ���� *************************
	a = 1.11112f;
	result = 12345.0f;

	StartTimer();

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Div(result, a);
	}

	pastTime = GetPastSeconds();
	Log("Past Time : %fs, Result : %f", pastTime, result);	// ����Ҫ���һ��result,������������Զ��Ż����޸�δʹ�ñ��������

	// ************************* �ӷ� *************************
	a = 1.11112f;
	result = 12345.0f;

	StartTimer();

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Add(result, a);
	}

	pastTime = GetPastSeconds();
	Log("Past Time : %fs, Result : %f", pastTime, result);	// ����Ҫ���һ��result,������������Զ��Ż����޸�δʹ�ñ��������

	// ************************* ���� *************************
	a = 1.11112f;
	result = 12345.0f;

	StartTimer();

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Sub(result, a);
	}

	pastTime = GetPastSeconds();
	Log("Past Time : %fs, Result : %f", pastTime, result);	// ����Ҫ���һ��result,������������Զ��Ż����޸�δʹ�ñ��������

	system("pause");

	return 0;
}