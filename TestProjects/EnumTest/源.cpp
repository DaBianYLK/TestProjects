#include <iostream>
#include <string>

using namespace std;

// 测试Enum和Int之间的转换

// 测试结果：
//		1. 枚举类型可以直接赋值给整数
//		2. 整数不可以直接赋值给枚举类型变量
//		3. 整数可以通过静态转换赋值给枚举类型
//		4. 整数不可以通过重解释转换赋值给枚举类型

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

	// TestEnum e3 = reinterpret_cast<TestEnum>(3);	// VS2013报错，无法执行此转换
	// int y = 3;
	// TestEnum e3 = reinterpret_cast<TestEnum>(y);	// VS2013报错，无法执行此转换

	TestEnum e4 = (TestEnum)4;

	// TestEnum e5 = 5;	// VS2013报错，无法直接对枚举类型使用整数赋值

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