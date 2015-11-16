#include <iostream>

using namespace std;

// RTTI : ����ʱ������Ϣ���ڡ����� -> ���� -> C/C++ -> ���� -> ����ʱ������Ϣ���п�����

// 1. ���Զ�̬����£�ͨ��ָ���typeid��ȡ���������Ƿ����
// 2. ���Զ�̬����£��ܷ�ͨ������ָ�����ֱ�ӷ��ʺ����ĸ���汾

// ���Խ����
//		1. �����������麯�����ſ���ʹ�û���ָ���ȡ����������
//		   ������������ʱ��A�����麯����B�������麯����ֻ��ʹ��A��ָ�������ȷ��ȡ�����������
//		2. �ڻ���Ĺ��캯���У�ͨ��thisָ�벻�ܻ�ȡ����������
//		   ��������ɺ����ͨ��thisָ���ȡ����������
//		3. �������۶Խṹ��ͬ������

// ע��ʹ�ö�̬ʱ��Ҫע���̬�ǲ�֧���ø����������������ģ���������������Ծ����ģ�������ÿ���ڴ�Ĵ�С����һ����
//	   ���������ʹ�������ָ������

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

	cout << "����˵����" << endl;
	cout << "����A���������麯��" << endl;
	cout << "����B��û�������麯��" << endl;

	// ����Ŀ�ģ�typeid�ܷ�ʶ��ָ�������
	// ���Խ������
	cout << "ͨ��typeid��ȡָ����ָ���������ͣ�" << endl;
	cout << "����A��ָ�루ָ�����A����: " << typeid(*pBaseClassA).name() << endl;
	cout << "����B��ָ�루ָ�����B����: " << typeid(*pBaseClassB).name() << endl;
	cout << "�� ���ָ�루ָ���� �����: " << typeid(*pDerivedClass).name() << endl;
	cout << endl;

	// ����Ŀ�ģ���̬ʱ�Ҷ�̳�ʱ��ͨ��typeid�ܷ���ȷ��ȡָ����ָ���ʵ�ʶ�������
	// ���Խ��������
	// ע���޷�ͨ��voidָ�뱣��������Ϣ
	cout << "��̬�Ҷ�̳�ʱ��" << endl;
	cout << "����A��ָ�루ָ���������: " << typeid(*pPolyBaseClassA).name() << endl;
	cout << "����B��ָ�루ָ���������: " << typeid(*pPolyBaseClassB).name() << endl;
	cout << "V O I Dָ�루ָ���������: " << typeid(voidPtr).name() << endl;
	cout << endl;

	// ����Ŀ�ģ���̬ʱ�Ҷ�̳�ʱ��ͨ��typeid�ܷ���ȷ��ȡ���ö����ʵ������
	// ���Խ��������
	cout << "��̬�Ҷ�̳�ʱ��" << endl;
	cout << "����A�����ã�ͨ���������ֵ��: " << typeid(polyBaseClassAObject).name() << endl;
	cout << "����B�����ã�ͨ���������ֵ��: " << typeid(polyBaseClassBObject).name() << endl;
	cout << endl;

	// ����Ŀ�ģ���̬ʱ���ڻ���Ĺ��캯����ʹ��thisָ���ܷ��ȡ��������
	// ���Խ����������û��๹�캯��ʱ���ڻ��๹�캯����ʹ��thisָ���ȡ���ǻ�������
	cout << "�ڻ��๹�캯���У����๹�캯����ֱ�ӻ�ȡthisָ������������ܷ�ͨ��thisָ���ȡ��ǰ��������ͣ�" << endl;
	cout << "����A��thisָ�루ָ��������: " << baseClassAObject.name.c_str() << endl;
	cout << "����A��thisָ�루ָ���������: " << derivedClassObject.name.c_str() << endl;
	cout << endl;

	// ����Ŀ�ģ���̬ʱ����thisָ�븳ֵ����Ա�������������ܷ��ȡ���������
	// ���Խ�������๹����ɺ󣬿���ͨ��thisָ��ָ��ĵ�ַ��ȡ��������
	cout << "�ڻ��๹�캯���⣨��thisָ�븳ֵ����Ա������������ͨ���ó�Ա������ȡ���������ܷ�ͨ��thisָ���ȡ��ǰ��������ͣ�" << endl;
	cout << "����A��thisָ�루ָ��������:" << typeid(*baseClassAObject.pThis).name() << endl;
	cout << "����A��thisָ�루ָ���������:" << typeid(*derivedClassObject.pThis).name() << endl;
	cout << endl;

	// ����Ŀ�ģ���̬ʱ���ܷ�ͨ�����޶�������ȷ���麯���汾
	// ���Խ����ͨ��
	cout << "��̬ʱ���ܷ�ͨ�����޶�������ȷ�ĺ����汾��" << endl;
	cout << "ͨ������A��ָ�루ָ��������󣩵����麯�����������޶�: " << endl;
	pPolyBaseClassA->DoSomeThing();
	cout << "ͨ������A��ָ�루ָ��������󣩵����麯�����ӻ������޶�: " << endl;
	pPolyBaseClassA->BaseClassA::DoSomeThing();
	cout << endl;

	// ����Ŀ�ģ���̬ʱ�����麯���ĵ������
	// ���Խ�������麯�������ָ�������ѡ����õİ汾
	cout << "��̬ʱ���ܷ�ͨ�����޶�������ȷ�ĺ����汾��" << endl;
	cout << "ͨ������A��ָ�루ָ��������󣩵��÷��麯�����������޶�: " << endl;
	pPolyBaseClassA->DoAnotherThing();
	cout << "ͨ������A��ָ�루ָ��������󣩵��÷��麯�����ӻ������޶�: " << endl;
	pPolyBaseClassA->BaseClassA::DoAnotherThing();
	cout << endl;
	
	BaseStruct* pBaseStructObject;
	DerivedStruct* pDerivedStructObject = new DerivedStruct();

	pBaseStructObject = pDerivedStructObject;

	// ����Ŀ�ģ��ṹ���Ƿ���ڶ�̬
	// ���Խ��������
	cout << "�ṹ���Ƿ���ڶ�̬���ԣ�" << endl;
	cout << "���ṹָ�루ָ���ӽṹ����: " << typeid(*pBaseStructObject).name() << endl;
	cout << "�ӽṹָ�루ָ���ӽṹ����: " << typeid(*pDerivedStructObject).name() << endl;
	cout << endl;

	// ����Ŀ�ģ���̳�ʱ��delete�ڶ��������ָ���Ƿ������ڴ�й¶
	// ���Խ��������
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