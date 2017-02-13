#pragma once

#include <string>
#include <iostream>

using namespace std;

struct TestStruct
{
	string strName;

	TestStruct()
	{
		cout << "TestStruct Created." << endl;
	}

	static TestStruct inDefineTest;
};


extern TestStruct outDefineTest;