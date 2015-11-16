#include <iostream>

using namespace std;

// RTTI : 运行时类型信息（在“工程 -> 属性 -> C/C++ -> 语言 -> 运行时类型信息”中开启）

// 1. 测试多态情况下，通过指针和typeid获取派生类名是否可行
// 2. 测试多态情况下，能否通过子类指针和域直接访问函数的父类版本

// 测试结果：
//		1. 基类必须包含虚函数，才可以使用基类指针获取派生类类型
//		   当有两个基类时，A包含虚函数，B不包含虚函数，只有使用A的指针才能正确获取派生类的类名
//		2. 在基类的构造函数中，通过this指针不能获取派生类类名
//		   对象构造完成后可以通过this指针获取派生类类名
//		3. 上述结论对结构体同样适用

// 注：使用多态时，要注意多态是不支持用父类数组存放子类对象的，这是由数组的特性决定的（数组中每块内存的大小必须一样）
//	   解决方法是使用链表或指针数组

class BaseClassA {
public:
	BaseClassA() {
		name = typeid(*this).name();	

		pThis = this;
	}

	virtual ~BaseClassA() {

	}

	virtual void DoSomeThing() {
		cout << "Call Base Class A to do something." << endl;
	}

	void DoAnotherThing() {
		cout << "Call Base Class A to do anotherthing." << endl;
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

class DerivedClassA : public BaseClassA {
public:
	DerivedClassA() {
		x = 0;
	}

	~DerivedClassA() {

	}

	int x;
};

class DerivedClass : public BaseClassA, public BaseClassB {
public:
	DerivedClass() {
		x = 0;
	}

	~DerivedClass() {

	}

	void DoSomeThing() {
		cout << "Call Derived Class to do something." << endl;
	}

	void DoAnotherThing() {
		cout << "Call Derived Class to do anotherthing." << endl;
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
	BaseClassA baseClassAObject;
	BaseClassB baseClassBObject;
	DerivedClass derivedClassObject;

	BaseClassA* pBaseClassA = &baseClassAObject;
	BaseClassB* pBaseClassB = &baseClassBObject;
	DerivedClass* pDerivedClass = &derivedClassObject;

	BaseClassA* pPolyBaseClassA = &derivedClassObject;
	BaseClassA& polyBaseClassAObject = derivedClassObject;

	BaseClassB* pPolyBaseClassB = &derivedClassObject;
	BaseClassB& polyBaseClassBObject = derivedClassObject;

	void* voidPtr = &derivedClassObject;

	cout << "测试说明：" << endl;
	cout << "基类A：声明了虚函数" << endl;
	cout << "基类B：没有声明虚函数" << endl;

	// 测试目的：typeid能否识别指针的类型
	// 测试结果：能
	cout << "通过typeid获取指针所指向对象的类型：" << endl;
	cout << "基类A的指针（指向基类A对象）: " << typeid(*pBaseClassA).name() << endl;
	cout << "基类B的指针（指向基类B对象）: " << typeid(*pBaseClassB).name() << endl;
	cout << "子 类的指针（指向子 类对象）: " << typeid(*pDerivedClass).name() << endl;
	cout << endl;

	// 测试目的：多态时且多继承时，通过typeid能否正确获取指针所指向的实际对象类型
	// 测试结果：可以
	// 注：无法通过void指针保存类型信息
	cout << "多态且多继承时：" << endl;
	cout << "基类A的指针（指向子类对象）: " << typeid(*pPolyBaseClassA).name() << endl;
	cout << "基类B的指针（指向子类对象）: " << typeid(*pPolyBaseClassB).name() << endl;
	cout << "V O I D指针（指向子类对象）: " << typeid(voidPtr).name() << endl;
	cout << endl;

	// 测试目的：多态时且多继承时，通过typeid能否正确获取引用对象的实际类型
	// 测试结果：可以
	cout << "多态且多继承时：" << endl;
	cout << "基类A的引用（通过子类对象赋值）: " << typeid(polyBaseClassAObject).name() << endl;
	cout << "基类B的引用（通过子类对象赋值）: " << typeid(polyBaseClassBObject).name() << endl;
	cout << endl;

	// 测试目的：多态时，在基类的构造函数中使用this指针能否获取子类类型
	// 测试结果：子类调用基类构造函数时，在基类构造函数中使用this指针获取的是基类类型
	cout << "在基类构造函数中（基类构造函数中直接获取this指针的类型名）能否通过this指针获取当前对象的类型：" << endl;
	cout << "基类A的this指针（指向基类对象）: " << baseClassAObject.name.c_str() << endl;
	cout << "基类A的this指针（指向子类对象）: " << derivedClassObject.name.c_str() << endl;
	cout << endl;

	// 测试目的：多态时，将this指针赋值给成员变量，在类外能否获取子类的类型
	// 测试结果：子类构造完成后，可以通过this指针指向的地址获取子类类型
	cout << "在基类构造函数外（将this指针赋值给成员变量，在类外通过该成员变量获取类型名）能否通过this指针获取当前对象的类型：" << endl;
	cout << "基类A的this指针（指向基类对象）:" << typeid(*baseClassAObject.pThis).name() << endl;
	cout << "基类A的this指针（指向子类对象）:" << typeid(*derivedClassObject.pThis).name() << endl;
	cout << endl;

	// 测试目的：多态时，能否通过域限定访问正确的虚函数版本
	// 测试结果：通过
	cout << "多态时，能否通过域限定访问正确的函数版本：" << endl;
	cout << "通过基类A的指针（指向子类对象）调用虚函数，不加域限定: " << endl;
	pPolyBaseClassA->DoSomeThing();
	cout << "通过基类A的指针（指向子类对象）调用虚函数，加基类域限定: " << endl;
	pPolyBaseClassA->BaseClassA::DoSomeThing();
	cout << endl;

	// 测试目的：多态时，非虚函数的调用情况
	// 测试结果：非虚函数会根据指针的类型选择调用的版本
	cout << "多态时，能否通过域限定访问正确的函数版本：" << endl;
	cout << "通过基类A的指针（指向子类对象）调用非虚函数，不加域限定: " << endl;
	pPolyBaseClassA->DoAnotherThing();
	cout << "通过基类A的指针（指向子类对象）调用非虚函数，加基类域限定: " << endl;
	pPolyBaseClassA->BaseClassA::DoAnotherThing();
	cout << endl;
	
	BaseStruct* pBaseStructObject;
	DerivedStruct* pDerivedStructObject = new DerivedStruct();

	pBaseStructObject = pDerivedStructObject;

	// 测试目的：结构体是否存在多态
	// 测试结果：存在
	cout << "结构体是否存在多态特性：" << endl;
	cout << "基结构指针（指向子结构对象）: " << typeid(*pBaseStructObject).name() << endl;
	cout << "子结构指针（指向子结构对象）: " << typeid(*pDerivedStructObject).name() << endl;
	cout << endl;

	// 测试目的：多继承时，delete第二个基类的指针是否会造成内存泄露
	// 测试结果：不会
	/*for (int i = 0; i < 10000000; ++i) {
		pDerivedClass = new DerivedClass();
		printf("allocated at address %p\n", pDerivedClass);

		pBaseClassB = pDerivedClass;
		printf("delete at address %p\n", pBaseClassB);

		delete pBaseClassB;
	} */

	system("pause");

	return;
}