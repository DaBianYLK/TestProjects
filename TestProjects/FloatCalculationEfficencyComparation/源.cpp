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
 * Releaseģʽ�¼�ʱ�����
 *		�˷� 0.014785s
 *		���� 0.038151s
 *		�ӷ� 0.008867s
 *		���� 0.008732s
 *
 * ���������
 *		1. �������Ӽ���Ч����������Ϊ�˷�������Ч�����
 *		2. ��Releaseģʽ�£��������Ӽ������˷��������ʱ�����ı�ԼΪ 1 : 2 : 5
		3. ע����������ȡĳЩ����ֵʱ��������쳣��ʱ��ͳ�ƽ����ԭ����
*
* ���ۣ�
*		��ϣ��Ч����󻯵�����£���Ҫ�������⸡�����������㣺
*		1. ����Ҫ�󲻸�ʱ���Ը�Ϊ���������������λ����
*		2. ��γ���ͬһ������ʱ����ת��Ϊ��������ʹ�ó˷�
 */

const unsigned int nNumberCount = 10000000;	// 1ǧ���int/float��ռ��40M�ڴ�

float	fResult = 1000.0f;
int		nResult = 1000;

float*	fArray;
int*	iArray;

inline void IntMul() {
	for (unsigned int i = 0; i < nNumberCount; ++i) {
		nResult *= iArray[i];
	}
}

inline void Mul() {
	for (unsigned int i = 0; i < nNumberCount; ++i) {
		fResult *= fArray[i];
	}
}

inline void Div() {
	for (unsigned int i = 0; i < nNumberCount; ++i) {
		fResult /= fArray[i];
	}
}

inline void Add() {
	for (unsigned int i = 0; i < nNumberCount; ++i) {
		fResult += fArray[i];
	}
}

inline void Sub() {
	for (unsigned int i = 0; i < nNumberCount; ++i) {
		fResult -= fArray[i];
	}
}

int main(void) {

	fArray = new float[nNumberCount];
	iArray = new int[nNumberCount];

	for (int i = 0; i < nNumberCount; ++i)
	{
		fArray[i] = 1.000001f;
		iArray[i] = 3;
	}

	Log("Init : %f %d", fArray[0], iArray[0]);

	// ************************* �˷� *************************
	BENCHMARK(Test, Mul());
	Log("Result : %f", fResult);			// ����Ҫ���һ��fResult��������������Ż���ɾ��ִ������Ĵ���

	system("pause");

	// ************************* �˷� *************************
	BENCHMARK(Multiple, Mul());
	Log("Result : %f", fResult);			// ����Ҫ���һ��fResult��������������Ż���ɾ��ִ������Ĵ���

	// ************************* ���� *************************
	fResult = 1000.0f;

	BENCHMARK(Division, Div());
	Log("Result : %f", fResult);			// ����Ҫ���һ��fResult��������������Ż���ɾ��ִ������Ĵ���


	// ************************* �ӷ� *************************
	fResult = 1000.0f;

	BENCHMARK(Addition, Add());
	Log("Result : %f", fResult);			// ����Ҫ���һ��fResult��������������Ż���ɾ��ִ������Ĵ���

	// ************************* ���� *************************
	fResult = 1000.0f;

	BENCHMARK(Subtraction, Sub());
	Log("Result : %f", fResult);			// ����Ҫ���һ��fResult��������������Ż���ɾ��ִ������Ĵ���

	BENCHMARK(IntMul, IntMul());
	Log("Result : %d", nResult);			// ����Ҫ���һ��fResult��������������Ż���ɾ��ִ������Ĵ���

	system("pause");

	return 0;
}