#include <iostream>
#include <winnt.h>

using namespace std;

void main()
{
	const char* strTestTextASCII = "ASCII char set test. ASCII �ַ�������ԡ� %s - %d";
	const wchar_t* strTestTextUnicode = L"Unicode char set test. Unicode �ַ�������ԡ� %s - %d" ;

	printf_s(strTestTextASCII, __FILE__, __LINE__);
	printf_s("\n");

	_wsetlocale(LC_ALL, L"chs");							// ָ�����룬�����޷�ͨ�����ַ���ʾ����
	wprintf_s(strTestTextUnicode, __FILEW__, __LINE__);		// __FILE__��ȡ����ASCII�ַ�������Ҫͨ��TEXT(__FILE__)����ת��
	wprintf_s(L"\n");

	system("pause");
}