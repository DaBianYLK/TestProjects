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
	Release�е�����pObject��ָ����������������������������������У���ȻpObject��ָ����ڴ�ռ�û�б��ͷţ�
	����������������������ú󣬻�ָ�������麯������ʱpObjectָ���ʧȥ�˶�̬���ԣ�֮����Ҳ�޷���ͨ������ָ��pObjectֱ�ӵ�����������麯����
	*/
	pObject->Release();

	// �ֶ����������������ڴ�ռ䲢û�б��ͷ�
	((Derive*)pObject)->deriveData = 30;
	cout << pObject->baseData << endl;
	cout << ((Derive*)pObject)->deriveData << endl;

	// ��ʱɾ��pObject��ֻ����û����������������Ϊ֮ǰ�Ѿ��ֶ����ù��������������麯�����Ѿ������า��
	delete pObject;

	// ���ܽ᡿ �ڶ�̬������£���������ֻ���ֶ�����һ�Σ����������޷�Ԥ֪�Ľ��

	system("pause");

	return 0;
}