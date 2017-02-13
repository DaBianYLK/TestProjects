#include <iostream>

using namespace std;

__declspec(align(16)) struct AlignedVector
{
	float x, y, z, w;
};

struct Vector
{
	float x, y, z, w;
};

struct TestA
{
	int b;
	AlignedVector v;
	int a;
};

struct TestB
{
	int b;
	Vector v;
	int a;
};

struct UnsignedChar
{
	unsigned char u1 : 1;
	unsigned char u2 : 1;
	unsigned char u3 : 1;
	unsigned char u4 : 1;
	unsigned char u5 : 1;

	unsigned char : 0;
};

int main()
{
	cout << sizeof(AlignedVector) << endl;
	cout << sizeof(Vector) << endl;
	cout << sizeof(TestA) << endl;
	cout << sizeof(TestB) << endl;

	cout << sizeof(UnsignedChar) << endl;

	system("pause");
	return 0;
}