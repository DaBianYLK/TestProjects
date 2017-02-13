#include <iostream>
#include <string>

using namespace std;

int main()
{
	string strSize = "\0";
	
	cout << strSize.size() << endl;

	strSize.resize(3);
	
	const char* szSize = "abc";

	memcpy_s(&strSize[0], 3, szSize, 3);

	char* szTemp = &strSize[0];
	cout << (int)szTemp[3] << endl;

	cout << strSize.c_str() << endl;
	cout << strSize.size() << endl;

	system("pause");
	return 0;
}