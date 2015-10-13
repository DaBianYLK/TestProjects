#include <iostream>
#include <string>

using namespace std;

// 测试使用memcpy结合const_cast复制string的可行性

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