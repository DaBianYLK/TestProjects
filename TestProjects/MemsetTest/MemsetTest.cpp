#include <iostream>

using namespace std;

struct TestStruct
{
	int s32A;
	int s32B;
	int s32C;
	int s32D;
};

union TestUnion
{
	TestStruct abcd[2];
	char szName[32];
};
	
struct TestNewStruct
{
	int s32A;
	int s32B;
	int s32C;
	int s32D;

	int* pArray;

	

	TestNewStruct()
	{
		s32A = 0;
	}
};

void main()
{
	TestUnion test;
	test.szName[0] = 't';
	test.szName[1] = 'e';
	test.szName[2] = 's';
	test.szName[3] = 't';
	test.szName[4] = '\0';

	TestNewStruct aaa[3];

	//memset(&test.abcd, 0, sizeof(TestStruct));

	cout << reinterpret_cast<int>(test.abcd) << endl;
	cout << reinterpret_cast<int>(&test.abcd) << endl;

	cout << test.szName << endl;

	TestUnion* pTest = new TestUnion;
	pTest->szName[0] = 't';
	pTest->szName[1] = 'e';
	pTest->szName[2] = 's';
	pTest->szName[3] = 't';
	pTest->szName[4] = '\0';

	//memset(&test.abcd, 0, sizeof(TestStruct));

	cout << reinterpret_cast<int>(pTest->abcd) << endl;
	cout << reinterpret_cast<int>(&pTest->abcd) << endl;

	cout << pTest->szName << endl;

	TestNewStruct newTest;
	newTest.pArray = new int[32];

	cout << reinterpret_cast<int>(newTest.pArray) << endl;
	cout << reinterpret_cast<int>(&newTest.pArray) << endl;

	system("pause");
}