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
* ����Ŀ�ģ��жϼӷ���������Ч��
*
* ���������
*		1. Releaseģʽ�¸�����1�ڴΣ��ӷ���ʱ0.031s���ң��������ʱ0.029s����
*		2. Debugģʽ�¸�����1�ڴΣ��ӷ���ʱ0.226s���ң��������ʱ0.217s����
*
* ���ۣ�
*		������Ч���Ը��ڼӷ����㡣
*/

const unsigned int nCalculation = 100000000;
unsigned long long resultNumber = 0;
unsigned int addNumber = 16;

inline void AddOperation() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		resultNumber += addNumber;
	}
}

inline void OrOperation() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		resultNumber |= addNumber;
	}
}

int main(void) {

	// ************************* �ӷ� *************************
	BENCHMARK(AddOperation, AddOperation());
	Log("Result : %ulld", resultNumber);			// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	// ************************* ������ *************************
	resultNumber = 0;
	addNumber = 16;

	BENCHMARK(OrOperation, OrOperation());
	Log("Result : %ulld", resultNumber);			// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���

	system("pause");

	return 0;
}