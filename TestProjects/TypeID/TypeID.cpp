#include <iostream>
#include <string>

using namespace std;

struct A
{
	virtual ~A()
	{
		cout << "A has been destructed." << endl;
	};

	virtual void OutputInfo()
	{
		cout << "A" << endl;
	}

	int data;
};

struct B : public A
{
	~B()
	{
		cout << "B has been destructed." << endl;
	};

	void OutputInfo()
	{
		cout << "B" << endl;
	}

	int bData;
	string strData;
};

struct C : public B
{
	~C()
	{
		cout << "C has been destructed." << endl;
	};

	void OutputInfo()
	{
		cout << "C" << endl;
	}

	int data;
};

class TestClass : public A
{
public:
	TestClass()
	{
		new(this) TestClass(20);
	}
	TestClass(int i)
	{
		data = i;
	}

	int data;
};

template<typename T>
void OutputTypeName(const T& pObject)
{
	cout << typeid(T).name() << endl;
	cout << typeid(pObject).name() << endl;
}

int main()
{
	A a;
	A& rA = a;
	A* pA = &a;
	const A* const_pA = &a;

	A* pB = new C;

	//OutputTypeName(*pB);

	pB->OutputInfo();

	delete[] pB;

	A* pObject = new A[10];
	const A* pTemp = pObject;
	pTemp++;
	cout << pTemp;

	/*cout << reinterpret_cast<unsigned int>(typeid(A).name()) << endl;
	cout << reinterpret_cast<unsigned int>(typeid(a).name()) << endl;
	cout << reinterpret_cast<unsigned int>(typeid(rA).name()) << endl;
	cout << reinterpret_cast<unsigned int>(typeid(*pA).name()) << endl;
	cout << reinterpret_cast<unsigned int>(typeid(*const_pA).name()) << endl;

	A* p = new B[3];

	cout << typeid(*p).name() << endl;

	delete[] p;*/

	system("pause");

	return 0;
}