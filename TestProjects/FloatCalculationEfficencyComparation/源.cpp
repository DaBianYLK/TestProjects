#include <iostream>

using namespace std;

/* 
 * ����Ŀ�ģ��жϸ�������������֮���Ч�ʲ���
 *
 * Debugģʽ�²��Խ����
 *		ÿ�����㺯����CPUռ�ðٷֱȣ�
 *			�˷� 6.29%
 *			���� 42.86%
 *			�ӷ� 5.98%
 *			���� 6.32%
 *
 * ���������
 *		1. �������˷����Ӽ���Ч�����������Ч�����
 *		2. �������˷�Ч���������Ч�ʱ�ԼΪ1��7
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

	float a = 1.11112f;
	float result = 12345.0f;

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Mul(result, a);
	}

	a = 1.11112f;
	result = 12345.0f;

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Div(result, a);
	}

	a = 1.11112f;
	result = 12345.0f;

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Add(result, a);
	}

	a = 1.11112f;
	result = 12345.0f;

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Sub(result, a);
	}

	return 0;
}