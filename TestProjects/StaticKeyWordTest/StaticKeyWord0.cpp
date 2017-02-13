#include "StaticKeyWord0.h"
#include "StaticKeyWord1.h"
#include <Windows.h>

const unsigned int uInfinite = 99999;

unsigned int g_uDistances[7][7] =
	{
		{ 0,			7,			3,			uInfinite,	uInfinite,	uInfinite,	uInfinite },
		{ 7,			0,			3,			5,			uInfinite,	uInfinite,	uInfinite },
		{ 3,			3,			0,			3,			4,			uInfinite,	uInfinite },
		{ uInfinite,	5,			3,			0,			2,			3,			uInfinite },
		{ uInfinite,	uInfinite,	4,			2,			0,			5,			4		  },
		{ uInfinite,	uInfinite,	uInfinite,	3,			5,			0,			3		  },
		{ uInfinite,	uInfinite,	uInfinite,	uInfinite,	4,			3,			0		  }
	};

template<int nCount>
unsigned int Dijkstra(unsigned int uDistances[][nCount])
{
	for (int i = 1; i < nCount; ++i)
	{
		for (int j = i + 1; j < nCount; ++j)
		{
			unsigned int uDistance = uDistances[0][i] + uDistances[i][j];
			if (uDistances[0][j] > uDistance)
			{
				uDistances[0][j] = uDistance;
			}
		}
	}

	return uDistances[0][nCount - 1];
}

class A
{
public:
	int 				nX;
	bool 			bY;
	float				fZ;
	unsigned long long 	uW;
};


int main()
{
	system("start /b /wait call \"0032. ParameterCountProfile.exe\"");
	//WinExec("\"0032. ParameterCountProfile.exe\"", SW_HIDE);

	cout << Dijkstra(g_uDistances) << endl;

	cout << "Main Function Started." << endl;

	outDefineTest.strName = "Test From 0";
	TestStruct::inDefineTest.strName = "Test From 0";
	SetTestName("Test From 1");

	cout << outDefineTest.strName << endl;
	cout << TestStruct::inDefineTest.strName << endl;
	OutputTestName();
	A a;
	printf("%d\n%d\n%d\n%d\n%d", sizeof(A), (int)&a.nX, (int)&a.bY, (int)&a.fZ, (int)&a.uW);
	system("pause");
	return 0;
}
