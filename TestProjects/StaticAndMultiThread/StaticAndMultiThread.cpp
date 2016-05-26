/* 
	Ŀ�ģ���֤static�ڶ��߳����Ƿ�ȫ

	�����VS2013�ܱ�֤static�����ڶ��߳���ֻ������һ�Σ������ܱ�֤���ص�static�����Ѿ�������ɡ�
	Ps����˵VS2015�����������⡣
*/

#include <iostream>
#include <Windows.h>

using namespace std;

class ClassA
{
public:
	ClassA()
	{
		cout << "Construct ClassA Begin." << endl;
		for (int i = 0; i < 500000000; ++i)
		{
			nData += 2;
		}
		cout << "Construct ClassA End." << endl;
	}

	~ClassA()
	{
		
	}

	static ClassA& GetSingleton()
	{
		static ClassA instance;
		return instance;
	}

	int nData;
};

DWORD WINAPI DoSomething(LPVOID lpParam)
{
	ClassA* a = &ClassA::GetSingleton();

	cout << a << endl;

	return 0;
}

void main()
{
	const int nThreadCount = 20;
	HANDLE hThreadArray[nThreadCount];

	for (int i = 0; i < nThreadCount; ++i)
	{
		hThreadArray[i] = CreateThread(NULL, 0, DoSomething, NULL, 0, NULL);
	}

	system("pause");
}