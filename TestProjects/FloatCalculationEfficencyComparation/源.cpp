#ifdef _DEBUG
#pragma comment(lib, "TestUtilsd.lib")
#else
#pragma comment(lib, "TestUtils.lib")
#endif

#include "../TestUtils/DB_Log.h"
#include "../TestUtils/DB_Timer.h"

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
 *		2. 在Release模式下，浮点数乘法效率与除法的时间消耗比约为 1 ： 30
*
* 结论：
*		在希望效率最大化的情况下，需要尽量避免浮点数除法运算：
*		1. 精度要求不高时可以改为整数除法运算或移位运算
*		2. 多次除以同一个变量时，先转化为倒数，再使用乘法
 */

const unsigned int nCalculation = 99999999;
float factor = 1.11112f;
float result = 12345.0f;

int nfactor = 2;
int nResult = 1;

inline void IntMul() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		nResult = nResult * nfactor;
	}
}

inline void Mul() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = result * factor;
	}
}

inline void Div() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = result / factor;
	}
}

inline void Add() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = result + factor;
	}
}

inline void Sub() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = result - factor;
	}
}

int main(void) {

	// ************************* 乘法 *************************
	BENCHMARK(Multiple, Mul());
	Log("Result : %f", result);			// 这里要输出一次result,否则编译器的优化会删除执行运算的代码

	// ************************* 除法 *************************
	factor = 1.11112f;
	result = 12345.0f;

	BENCHMARK(Division, Div());
	Log("Result : %f", result);			// 这里要输出一次result,否则编译器的优化会删除执行运算的代码


	// ************************* 加法 *************************
	factor = 1.11112f;
	result = 12345.0f;

	BENCHMARK(Addition, Add());
	Log("Result : %f", result);			// 这里要输出一次result,否则编译器的优化会删除执行运算的代码

	// ************************* 减法 *************************
	factor = 1.11112f;
	result = 12345.0f;

	BENCHMARK(Subtraction, Sub());
	Log("Result : %f", result);			// 这里要输出一次result,否则编译器的优化会删除执行运算的代码

	BENCHMARK(IntMul, IntMul());
	Log("Result : %d", nResult);			// 这里要输出一次result,否则编译器的优化会删除执行运算的代码

	system("pause");

	return 0;
}