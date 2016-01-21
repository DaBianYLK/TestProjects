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

const unsigned g_uCalculationCount = 400000000;

const unsigned g_uMid = g_uCalculationCount / 2;

unsigned g_uA = 0;
unsigned g_uB = 0;

inline void SmallZero(int bA, unsigned& iA, unsigned& iB)
{
	if (bA > 0)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}


inline void Bigger(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA > bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

inline void BiggerEqual(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA >= bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

inline void Smaller(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA < bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

inline void SmallerEqual(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA <= bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

inline void Equal(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA == bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

inline void UnEqual(unsigned bA, unsigned bB, unsigned& iA, unsigned& iB)
{
	if (bA != bB)
	{
		++iA;
	}
	else
	{
		++iB;
	}
}

void SmallZeroOperator()
{
	int j = 0;
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		SmallZero(j, g_uA, g_uB);
		++j;
	}
}

void BiggerOperatorTrue()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		Bigger(g_uMid, i, g_uA, g_uB);
	}
}

void BiggerOperatorFalse()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		Bigger(i, g_uMid, g_uA, g_uB);
	}
}

void BiggerEqualOperatorTrue()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		BiggerEqual(g_uMid, i, g_uA, g_uB);
	}
}

void BiggerEqualOperatorFalse()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		BiggerEqual(i, g_uMid, g_uA, g_uB);
	}
}

void SmallerOperatorTrue()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		Smaller(i, g_uMid, g_uA, g_uB);
	}
}

void SmallerOperatorFalse()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		Smaller(g_uMid, i, g_uA, g_uB);
	}
}

void SmallerEqualOperatorTrue()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		SmallerEqual(i, g_uMid, g_uA, g_uB);
	}
}

void SmallerEqualOperatorFalse()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		SmallerEqual(g_uMid, i, g_uA, g_uB);
	}
}

void EqualOperatorTrue()
{
	for (unsigned i = 0, j = g_uCalculationCount; i < g_uCalculationCount; ++i, --j)
	{
		Equal(i, j, g_uA, g_uB);
	}
}

void EqualOperatorFalse()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		Equal(i, g_uCalculationCount, g_uA, g_uB);
	}
}

void UnEqualOperatorTrue()
{
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		UnEqual(i, g_uCalculationCount, g_uA, g_uB);
	}
}

void UnEqualOperatorFalse()
{
	int j = 0;
	for (unsigned i = 0; i < g_uCalculationCount; ++i)
	{
		UnEqual(i, j, g_uA, g_uB);
		++j;
	}
}

unsigned main(void) 
{
	system("pause");

	BENCHMARK(SmallZeroOperator, SmallZeroOperator());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(BiggerOperatorTrue, BiggerOperatorTrue());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(BiggerOperatorFalse, BiggerOperatorFalse());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(BiggerEqualOperatorTrue, BiggerEqualOperatorTrue());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(BiggerEqualOperatorFalse, BiggerEqualOperatorFalse());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(SmallerOperatorTrue, SmallerOperatorTrue());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(SmallerOperatorFalse, SmallerOperatorFalse());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(SmallerEqualOperatorTrue, SmallerEqualOperatorTrue());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(SmallerEqualOperatorFalse, SmallerEqualOperatorFalse());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(EqualOperatorTrue, EqualOperatorTrue());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(EqualOperatorFalse, EqualOperatorFalse());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(UnEqualOperatorTrue, UnEqualOperatorTrue());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	g_uA = 0;
	g_uB = 0;

	BENCHMARK(UnEqualOperatorFalse, UnEqualOperatorFalse());
	// ����Ҫ���һ��result,������������Ż���ɾ��ִ������Ĵ���
	Log("Result : %u, %u", g_uA, g_uB);

	system("pause");

	return 0;
}