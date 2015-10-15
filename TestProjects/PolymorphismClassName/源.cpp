#include <iostream>

using namespace std;

// RTTI : 运行时类型信息（在“工程 -> 属性 -> C/C++ -> 语言 -> 运行时类型信息”中开启）

// 测试多态情况下，通过指针和typeid获取派生类名是否可行

// 测试结果：
//		1. 基类必须包含虚函数，才可以使用基类指针获取派生类类型
//		   当有两个基类时，A包含虚函数，B不包含虚函数，只有使用A的指针才能正确获取派生类的类名
//		2. 在基类的构造函数中，通过this指针不能获取派生类类名
//		   对象构造完成后可以通过this指针获取派生类类名
//		3. 上述结论对结构体同样适用

class BaseClassA {
public:
	BaseClassA() {
		name = typeid(*this).name();	

		pThis = this;
	}

	virtual ~BaseClassA() {

	}

	string name;
	BaseClassA* pThis;
};

class BaseClassB {
public:
	BaseClassB() {

	}

	~BaseClassB() {

	}

	float f;
};

class DerivedClass : public BaseClassA, public BaseClassB {
public:
	DerivedClass() {
		x = 0;
	}

	~DerivedClass() {

	}

	int x;
};

struct BaseStruct {
	virtual void Poly() { x = 0; };

	int x;
};

struct DerivedStruct : public BaseStruct {
	int y;
};

void main() {
	BaseClassA baseA;
	BaseClassB baseB;
	DerivedClass derived;

	BaseClassA* pBaseA = &baseA;
	BaseClassB* pBaseB = &baseB;
	DerivedClass* pDerived = &derived;

	BaseClassA* pPolymorphismA = &derived;
	BaseClassA polymorphismA = derived;

	BaseClassB* pPolymorphismB = &derived;
	BaseClassB polymorphismB = derived;

	cout << baseA.name.c_str() << endl;
	cout << derived.name.c_str() << endl;

	cout << typeid(*baseA.pThis).name() << endl;
	cout << typeid(*derived.pThis).name() << endl;

	cout << typeid(*pBaseA).name() << endl;
	cout << typeid(*pBaseB).name() << endl;
	cout << typeid(*pDerived).name() << endl;

	cout << typeid(*pPolymorphismA).name() << endl;
	cout << typeid(polymorphismA).name() << endl;

	cout << typeid(*pPolymorphismB).name() << endl;
	cout << typeid(polymorphismB).name() << endl;

	BaseStruct* pBaseC;
	DerivedStruct* pDerivedC = new DerivedStruct();

	pBaseC = pDerivedC;

	cout << typeid(*pBaseC).name() << endl;
	cout << typeid(*pDerivedC).name() << endl;

	system("pause");

	return;
}