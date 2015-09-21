#pragma comment(lib, "TestUtils.lib")

#include "DBLog.h"
#include "DBTimer.h"

#include <iostream>

using namespace std;
using namespace DaBianYLK;

/* 
 * 测试目的：判断浮点数四则运算之间的效率差异
 *
 * 测试方法：运行四则运算各1亿次，比较消耗的时间
 *
 * Debug模式下Profile结果——每种运算函数的CPU占用百分比：
 *		乘法 6.29%
 *		除法 42.86%
 *		加法 5.98%
 *		减法 6.32%
 *
 * Release模式下计时结果：
 *		乘法 0.147805s
 *		除法 4.411531s
 *		加法 0.088647s
 *		减法 0.087332s
 *
 * 结果分析：
 *		1. 浮点数乘法、加减法效率相近，除法效率最低
 *		2. 在Release模式下，浮点数乘法效率与除法的效率比约为1比30
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
	float pastTime = 0.0f;

	// ************************* 乘法 *************************
	float a = 1.11112f;
	float result = 12345.0f;

	StartTimer();

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Mul(result, a);
	}

	pastTime = GetPastSeconds();
	Log("Past Time : %fs, Result : %f", pastTime, result);	// 这里要输出一次result,否则编译器会自动优化掉修改未使用变量的语句

	// ************************* 除法 *************************
	a = 1.11112f;
	result = 12345.0f;

	StartTimer();

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Div(result, a);
	}

	pastTime = GetPastSeconds();
	Log("Past Time : %fs, Result : %f", pastTime, result);	// 这里要输出一次result,否则编译器会自动优化掉修改未使用变量的语句

	// ************************* 加法 *************************
	a = 1.11112f;
	result = 12345.0f;

	StartTimer();

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Add(result, a);
	}

	pastTime = GetPastSeconds();
	Log("Past Time : %fs, Result : %f", pastTime, result);	// 这里要输出一次result,否则编译器会自动优化掉修改未使用变量的语句

	// ************************* 减法 *************************
	a = 1.11112f;
	result = 12345.0f;

	StartTimer();

	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = Sub(result, a);
	}

	pastTime = GetPastSeconds();
	Log("Past Time : %fs, Result : %f", pastTime, result);	// 这里要输出一次result,否则编译器会自动优化掉修改未使用变量的语句

	system("pause");

	return 0;
}