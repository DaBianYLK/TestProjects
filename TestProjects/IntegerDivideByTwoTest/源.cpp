#pragma comment(lib, "TestUtils.lib")

#include "../TestUtils/DB_Log.h"
#include "../TestUtils/DB_Timer.h"

#include <iostream>

using namespace std;
using namespace DaBianYLK;

/*
* ����Ŀ�ģ��жϱ������������������Ż�
*
* ���Է�������2�����������Լ���2����������Ϊ�������г��������1�ڴΣ��Ƚ����ĵ�ʱ��
*
* Releaseģʽ�¼�ʱ�����
*	1.��resultΪunsigned int����
*		����4		0.030316s
*		��λ2		0.030457s
*		����16		0.030316s
*		��λ4		0.030457s
*		����5000	0.234306s
*		����500		0.146523s
*
*	2.��resultΪint����
*		����4		0.117160s
*		��λ2		0.030250s	�������Ƿ��ţ�
*		����16		0.117160s
*		��λ4		0.030076s	�������Ƿ��ţ�
*		����5000	0.209628s
*		����500		0.206247s
*		��λ2		0.090318s	�����Ƿ��ţ�
*
* ���������
*		1. ����2���������ݻ��Զ��Ż�Ϊ��λ
*		2. �޷������ͳ���ĳЩ��������ʱ���ĵ�ʱ�������50%��ԭ����
*		3. ����������Ϊ�з�������ʱ�����������Ż�Ч�ʻή��
*		4. �޷������������У���λ�������ʱ�����ı�ԼΪ 1 : 5
*		5. �з������������У���λ�������ʱ�����ı�ԼΪ 1 : 7 �������Ƿ���ʱ��
*		6. �з������������У������Զ������λ�Ż�Ч�ʱȱ��������Զ��Ż���20%��25% �����Ƿ��ţ�
*		7. �з������������У����Ƿ��ŵ���λ�벻���Ƿ��ŵ���λʱ�����ı�Ϊ 1 �� 3
*
* ���ۣ�
*		��ϣ��Ч����󻯵�����£��޷�������������Բ������ֶ���λ�Ż����з�������������Ҫ�����ֶ���λ�Ż�
*/

int main(void) {
	unsigned int nCalculation = 99999999;
	//unsigned int result = 1111111;
	int result = -99999999;

	BENCHMARK_START(
		DivisionWithOptimization,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = result / 4;
		}
	);

	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Divide by 4 \t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(DivisionWithOptimization), result);

	BENCHMARK_START(
		ShiftWith2,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = result >> 2;
		}
	);

	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Shifted by 2 \t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(ShiftWith2), result);

	BENCHMARK_START(
		DivisionWith16,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = result / 4;
		}
	);

	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Divide by 16 \t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(DivisionWithOptimization), result);

	BENCHMARK_START(
		ShiftWith4,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = result >> 4;
		}
	);

	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Shifted by 4 \t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(ShiftWith4), result);

	BENCHMARK_START(
		DivisionWith50000,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = result / 50000;
		}
	);

	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Divide by 50000\t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(DivisionWith50000), result);

	BENCHMARK_START(
		DivisionWith5000,
	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = result / 5000;
	}
	);

	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Divide by 5000 \t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(DivisionWith5000), result);

	BENCHMARK_START(
		NegativeShiftWith2,
	for (unsigned int i = 0; i < nCalculation; ++i) {
		bool negtive = false;
		if (result <= 0) {
			result = -result;
			negtive = true;
		}

		result = result >> 2;

		if (negtive) {
			result = -result;
		}
	}
	);

	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Neg Shift by 2\t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(NegativeShiftWith2), result);

	system("pause");

	return 0;
}