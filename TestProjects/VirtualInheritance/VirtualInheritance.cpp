#include <iostream>

using namespace std;

class Base
{
public:
	Base() {};
	virtual ~Base() {};

	static int s32Base;
};

int Base::s32Base = 0;

class DeriveA : public virtual Base
{
public:
	DeriveA() {};
	~DeriveA()	{};

private:
	int s32A;
};

class DeriveB : public virtual Base
{
public:
	DeriveB() {};
	~DeriveB()	{};

private:
	int s32B;
};

class DeriveC : public DeriveA, public DeriveB
{
public:
	DeriveC() {};
	~DeriveC()	{};

private:
	int s32C;
};

void main()
{
	cout << typeid(Base).name() << " : " << sizeof(Base) << endl;
	cout << typeid(DeriveA).name() << " : " << sizeof(DeriveA) << endl;
	cout << typeid(DeriveB).name() << " : " << sizeof(DeriveB) << endl;
	cout << typeid(DeriveC).name() << " : " << sizeof(DeriveC) << endl;


	system("pause");
	return;
}