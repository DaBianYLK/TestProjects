#include <iostream>
#include <string>

using namespace std;

// ����ʹ��memcpy���const_cast����string�Ŀ�����

void main() {
	string source = "Test String.";
	string destination;

	destination.resize(source.size());

	memcpy(const_cast<char*>(destination.c_str()), source.c_str(), sizeof(char) * source.size());

	cout << destination.c_str() << endl;
	cout << "src length : " << source.size() << endl;
	cout << "des length : " << destination.size() << endl;

	system("pause");

	return;
}