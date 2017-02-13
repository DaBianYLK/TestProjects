#include "StaticKeyWord0.h"
#include "StaticKeyWord1.h"

TestStruct TestStruct::inDefineTest;
TestStruct outDefineTest;

void SetTestName(const char* szName)
{
	outDefineTest.strName = szName;
	TestStruct::inDefineTest.strName = szName;
}

void OutputTestName()
{
	cout << outDefineTest.strName << endl;
	cout << TestStruct::inDefineTest.strName << endl;
}