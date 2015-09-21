#include <iostream>

using namespace std;

/* 
 * 测试目的：判断浮点数四则运算之间的效率差异
 *
 * Debug模式下测试结果：
 *		每种运算函数的CPU占用百分比：
 *			乘法 6.29%
 *			除法 42.86%
 *			加法 5.98%
 *			减法 6.32%
 *
 * 结果分析：
 *		1. 浮点数乘法、加减法效率相近，除法效率最低
 *		2. 浮点数乘法效率与除法的效率比约为1比7
 */

inline float Mul(float a, float b) {
	return a * b;
}

inline float Div(float a, float b) {
	return a / b;
}

inline float Add(float a, float b) {
	return a + b;
}

inline float Sub(float a, float b) {
	return a - b;
}

int main(void) {
	const unsigned int nCalculation = 99999999;

	float a = 1.11112f;
	float result = 12345.0f;

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Mul(result, a);
	}

	a = 1.11112f;
	result = 12345.0f;

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Div(result, a);
	}

	a = 1.11112f;
	result = 12345.0f;

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Add(result, a);
	}

	a = 1.11112f;
	result = 12345.0f;

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Sub(result, a);
	}

	return 0;
}