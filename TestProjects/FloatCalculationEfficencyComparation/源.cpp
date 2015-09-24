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
 *		2. ��Releaseģʽ�£��������˷�Ч���������ʱ�����ı�ԼΪ 1 �� 30
*
* ���ۣ�
*		��ϣ��Ч����󻯵�����£���Ҫ�������⸡�����������㣺
*		1. ����Ҫ�󲻸�ʱ���Ը�Ϊ���������������λ����
*		2. ��γ���ͬһ������ʱ����ת��Ϊ��������ʹ�ó˷�
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

	// ************************* �˷� *************************
	float factor = 1.11112f;
	float result = 12345.0f;

	BENCHMARK_START(
		Multiple, 
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = Mul(result, factor);
		}
	);

	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Past Time : %fs, Result : %f", BENCHMARK_RESULT(Multiple), result);	

	// ************************* ���� *************************
	factor = 1.11112f;
	result = 12345.0f;

	BENCHMARK_START(
		Division,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = Div(result, factor);
		}
	);

	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Past Time : %fs, Result : %f", BENCHMARK_RESULT(Division), result);	

	// ************************* �ӷ� *************************
	factor = 1.11112f;
	result = 12345.0f;

	BENCHMARK_START(
		Addition,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = Add(result, factor);
		}
	);

	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Past Time : %fs, Result : %f", BENCHMARK_RESULT(Addition), result);	

	// ************************* ���� *************************
	factor = 1.11112f;
	result = 12345.0f;

	BENCHMARK_START(
		Subtraction,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = Sub(result, factor);
		}
	);

	Log("Past Time : %fs, Result : %f", BENCHMARK_RESULT(Subtraction), result);	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	system("pause");

	return 0;
}