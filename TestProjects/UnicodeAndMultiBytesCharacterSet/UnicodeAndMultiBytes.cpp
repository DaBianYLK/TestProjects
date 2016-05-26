#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\TestUtilsd.lib")
#else
#pragma comment(lib, "..\\Release\\TestUtils.lib")
#endif

#pragma execution_character_set("utf-16")

#include "..\\TestUtils\\DB_Log.h"
#include "..\\TestUtils\\DB_Timer.h"

/*
Unicode字符串需要使用宽字符类型储存，但Unicode并不是唯一使用宽字符储存的字符编码格式，
在Windows中，即使在项目中选择了Unicode编码，部分函数在处理宽字符时仍然是默认按照多字节字符集处理的：
如wcout / wprintf / wfstream等待，它们的名字前增加了前缀“w”，仅仅是表明它们支持输入宽字符类型的字符串而已。
此时需要做的是调用locale函数，设置程序的编码格式。

经过测试，调用printf时使用Unicode比多字节字符集效率更低
*/

#include <iostream>
#include <Windows.h>
#include <tchar.h>

using namespace std;

inline void RunCharacterSetTest()
{
	for (int i = 0; i < 10; ++i)
	{
		_tprintf(TEXT("Test Text. 测试文本。%d\n"), i);
	}
}

struct AAA
{
	float A;
	float* B;
	float C;
};

void main()
{
#ifdef _UNICODE
	setlocale(LC_CTYPE, "");
	//std::locale oNewLocale(std::locale(), "", std::locale::ctype);
	//std::locale::global(oNewLocale);
#endif 

	BENCHMARK(RunCharacterSetTest, RunCharacterSetTest());

	system("pause");
	return;
}