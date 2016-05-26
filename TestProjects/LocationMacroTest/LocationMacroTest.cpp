#include <iostream>
#include <winnt.h>

using namespace std;

void main()
{
	const char* strTestTextASCII = "ASCII char set test. ASCII 字符编码测试。 %s - %d";
	const wchar_t* strTestTextUnicode = L"Unicode char set test. Unicode 字符编码测试。 %s - %d" ;

	printf_s(strTestTextASCII, __FILE__, __LINE__);
	printf_s("\n");

	_wsetlocale(LC_ALL, L"chs");							// 指定编码，否则无法通过宽字符显示中文
	wprintf_s(strTestTextUnicode, __FILEW__, __LINE__);		// __FILE__获取的是ASCII字符串，需要通过TEXT(__FILE__)进行转换
	wprintf_s(L"\n");

	system("pause");
}