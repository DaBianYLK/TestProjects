#include <iostream>

using namespace std;

struct Test
{
	__forceinline Test()
	{
		fValue = 0;
		cout << "Test Constructed" << endl;
	}

	__forceinline Test(const Test& other)
	{
		fValue = other.fValue;
		cout << "Test Copying Constructed" << endl;
	}

	~Test()
	{
		cout << "Test Destructed" << endl;
	}

	__forceinline Test& operator = (const Test& other)
	{
		fValue = other.fValue;
		cout << "Test Copied" << endl;
		return *this;
	}

	__forceinline Test operator + (const Test& other) const
	{
		Test out;
		out.fValue = fValue + other.fValue;
		cout << "Test Add" << endl;
		return out;
	}

	__forceinline Test& operator += (const Test& other)
	{
		fValue += other.fValue;
		cout << "Test Add Self" << endl;
		return *this;
	}


	float fValue;
};

__forceinline Test ReturnValue()
{
	{
	Test out;
	cout << "ReturnValue" << endl;
	return out;
	}
}

__forceinline Test& ReturnReference()
{
	Test out;
	cout << "ReturnReference" << endl;
	return out;
}

int main()
{
	Test a;
	a = ReturnValue();
	cout << a.fValue << endl;

	cout << endl;

	Test b;
	b = ReturnReference();
	cout << b.fValue << endl;

	cout << endl;

	Test c;
	c = a + b;
	cout << c.fValue << endl;

	cout << endl;

	Test d = a + b;
	cout << d.fValue << endl;

	cout << endl;

	d += a;
	cout << d.fValue << endl;

	cout << endl;

	d = d + a;
	cout << d.fValue << endl;

	cout << endl;

	Test e = a + b + c + d;
	cout << e.fValue << endl;

	system("pause");
	return 0;
}