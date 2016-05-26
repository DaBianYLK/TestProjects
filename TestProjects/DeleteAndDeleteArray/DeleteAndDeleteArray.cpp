
#include <cstdlib>
#include <iostream>

using namespace std;

struct UserDefinedType
{
	UserDefinedType(int i)
	{
		pData = new char[i];
	}

	UserDefinedType()
	{
		pData = new char[100];
	}

	~UserDefinedType()
	{
		delete[] pData;
	}

	char* pData;
};


int main()
{
	cout << "Object" << endl;

	UserDefinedType* pObject = new UserDefinedType();
	cout << pObject << endl;
	delete pObject;

	cout << endl << "Array" << endl;

	pObject = new UserDefinedType[1];

	pObject = new UserDefinedType[1];
	cout << pObject << endl;
	delete[] pObject;

	system("pause");
}