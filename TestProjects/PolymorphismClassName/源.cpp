#include <iostream>

using namespace std;

// RTTI : ����ʱ������Ϣ���ڡ����� -> ���� -> C/C++ -> ���� -> ����ʱ������Ϣ���п�����

// ���Զ�̬����£�ͨ��ָ���typeid��ȡ���������Ƿ����

// ���Խ����
//		1. �����������麯�����ſ���ʹ�û���ָ���ȡ����������
//		   ������������ʱ��A�����麯����B�������麯����ֻ��ʹ��A��ָ�������ȷ��ȡ�����������
//		2. �ڻ���Ĺ��캯���У�ͨ��thisָ�벻�ܻ�ȡ����������
//		   ��������ɺ����ͨ��thisָ���ȡ����������
//		3. �������۶Խṹ��ͬ������

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