#include <iostream>

using namespace std;


class Base
{
public:
	virtual void Output()
	{
		cout << "Base" << endl;
	}
};

class Derive : protected Base
{
	void Output1()
	{
		cout << "Derive" << endl;
	}
};

int main()
{
	//Base* pBase = new Derive();		// �޷�ͨ�����룺�����ɶԲ��ɷ��ʵĻ������ת����
	//pBase->Output();


	system("pause");
	return 0;
}