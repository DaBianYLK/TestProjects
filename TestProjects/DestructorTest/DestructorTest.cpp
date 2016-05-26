#include <iostream>

using namespace std;

class Base
{
public:
	Base() {};
	virtual ~Base()
	{
		cout << "Base has been destructed. " << endl;
	}

	void Release()
	{
		this->~Base();
	}

	int baseData;
};

class Derive : public Base
{
public:
	Derive() {};
	virtual ~Derive()
	{
		cout << "Derive has been destructed. " << endl;
	}

	void Releaase()
	{
		this->~Derive();
	}

	int deriveData;
};


int main()
{
	Base* pObject = new Derive();
	pObject->baseData = 20;

	/*
	Release中调用了pObject所指向派生类对象的虚析构函数，在这个过程中，虽然pObject所指向的内存空间没有被释放，
	但派生类的析构函数被调用后，会恢复基类的虚函数表，此时pObject指针就失去了多态特性，之后再也无法再通过基类指针pObject直接调用派生类的虚函数。
	*/
	pObject->Release();

	// 手动调用析构函数后，内存空间并没有被释放
	((Derive*)pObject)->deriveData = 30;
	cout << pObject->baseData << endl;
	cout << ((Derive*)pObject)->deriveData << endl;

	// 此时删除pObject，只会调用基类的析构函数，因为之前已经手动调用过了析构函数，虚函数表已经被基类覆盖
	delete pObject;

	// 【总结】 在多态的情况下，析构函数只能手动调用一次，否则会出现无法预知的结果

	system("pause");

	return 0;
}