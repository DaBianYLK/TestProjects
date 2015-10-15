#include <iostream>
#include <string>

using namespace std;

// ����Enum��Int֮���ת��

// ���Խ����
//		1. ö�����Ϳ���ֱ�Ӹ�ֵ������
//		2. ����������ֱ�Ӹ�ֵ��ö�����ͱ���
//		3. ��������ͨ����̬ת����ֵ��ö������
//		4. ����������ͨ���ؽ���ת����ֵ��ö������

enum TestEnum {
	Enum0 = 0,
	Enum1 = 1,
	Enum2 = 2,
	Enum3 = 3,
	Enum4 = 4,
	Enum5 = 5
};

void main() {
	int x = Enum0;

	TestEnum e1 = Enum1;

	TestEnum e2 = static_cast<TestEnum>(2);

	// TestEnum e3 = reinterpret_cast<TestEnum>(3);	// VS2013�����޷�ִ�д�ת��
	// int y = 3;
	// TestEnum e3 = reinterpret_cast<TestEnum>(y);	// VS2013�����޷�ִ�д�ת��

	TestEnum e4 = (TestEnum)4;

	// TestEnum e5 = 5;	// VS2013�����޷�ֱ�Ӷ�ö������ʹ��������ֵ

	cout << "Int convert from Enum0 : " << x << endl;
	cout << "Value of Enum1 : " << e1 << endl;

	if (e2 == 2) {
		cout << "Value of TestEnum convert from 2 (static cast) : " << e2 << endl;
	}

	if (e4 == 4) {
		cout << "Value of TestEnum convert from 4 (default cast) : " << e4 << endl;
	}

	system("pause");

	return;
}