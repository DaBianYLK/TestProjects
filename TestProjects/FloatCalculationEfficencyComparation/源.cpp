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

const unsigned int nCalculation = 99999999;
float factor = 1.11112f;
float result = 12345.0f;

int nfactor = 2;
int nResult = 1;

inline void IntMul() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		nResult = nResult * nfactor;
	}
}

inline void Mul() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = result * factor;
	}
}

inline void Div() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = result / factor;
	}
}

inline void Add() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = result + factor;
	}
}

inline void Sub() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = result - factor;
	}
}

int main(void) {

	// ************************* �˷� *************************
	BENCHMARK(Multiple, Mul());
	Log("Result : %f", result);			// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	// ************************* ���� *************************
	factor = 1.11112f;
	result = 12345.0f;

	BENCHMARK(Division, Div());
	Log("Result : %f", result);			// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���


	// ************************* �ӷ� *************************
	factor = 1.11112f;
	result = 12345.0f;

	BENCHMARK(Addition, Add());
	Log("Result : %f", result);			// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	// ************************* ���� *************************
	factor = 1.11112f;
	result = 12345.0f;

	BENCHMARK(Subtraction, Sub());
	Log("Result : %f", result);			// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	BENCHMARK(IntMul, IntMul());
	Log("Result : %d", nResult);			// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	system("pause");

	return 0;
}