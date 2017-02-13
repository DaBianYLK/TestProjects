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
	//Base* pBase = new Derive();		// 无法通过编译：“不可对不可访问的基类进行转换”
	//pBase->Output();


	system("pause");
	return 0;
}