#include <string>
#include <iostream>
#include <atlstr.h>

using namespace std;

// 测试string和'\0'字符的关系

void main() {
	string strTest = "123456";
	unsigned uTestSize = strTest.size();

	cout << strTest << endl;
	cout << strTest.c_str() << endl;
	cout << uTestSize << endl;
	cout << endl;

	strTest[3] = '\0';
	uTestSize = strTest.size();

	cout << strTest << endl;
	cout << strTest.c_str() << endl;
	cout << uTestSize << endl;
	cout << endl;

	strTest.append("\0");
	uTestSize = strTest.size();

	cout << strTest << endl;
	cout << strTest.c_str() << endl;
	cout << uTestSize << endl;
	cout << endl;

	strTest.append("\0 789");
	uTestSize = strTest.size();

	cout << strTest << endl;
	cout << strTest.c_str() << endl;
	cout << uTestSize << endl;
	cout << endl;

	strTest.append(" 789");
	uTestSize = strTest.size();

	cout << strTest << endl;
	cout << strTest.c_str() << endl;
	cout << uTestSize << endl;
	cout << endl;

	CString strCTest = strTest.c_str();
	cout << strCTest << endl;

	system("pause");

	return;
}