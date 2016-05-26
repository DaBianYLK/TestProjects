/* 
	目的：验证static在多线程中是否安全

	结果：VS2013能保证static变量在多线程中只被构造一次，但不能保证返回的static变量已经构造完成。
	Ps：据说VS2015解决了这个问题。
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