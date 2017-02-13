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
 * Release模式下计时结果：
 *		乘法 0.014785s
 *		除法 0.038151s
 *		加法 0.008867s
 *		减法 0.008732s
 *
 * 结果分析：
 *		1. 浮点数加减法效率相近，其次为乘法，除法效率最低
 *		2. 在Release模式下，浮点数加减法、乘法与除法的时间消耗比约为 1 : 2 : 5
		3. 注：当浮点数取某些特殊值时，会出现异常的时间统计结果，原因不明
*
* 结论：
*		在希望效率最大化的情况下，需要尽量避免浮点数除法运算：
*		1. 精度要求不高时可以改为整数除法运算或移位运算
*		2. 多次除以同一个变量时，先转化为倒数，再使用乘法
 */

const unsigned int nNumberCount = 10000000;	// 1千万个int/float，占用40M内存

float	fResult = 1000.0f;
int		nResult = 1000;

float*	fArray;
int*	iArray;

inline void IntMul() {
	for (unsigned int i = 0; i < nNumberCount; ++i) {
		nResult *= iArray[i];
	}
}

inline void Mul() {
	for (unsigned int i = 0; i < nNumberCount; ++i) {
		fResult *= fArray[i];
	}
}

inline void Div() {
	for (unsigned int i = 0; i < nNumberCount; ++i) {
		fResult /= fArray[i];
	}
}

inline void Add() {
	for (unsigned int i = 0; i < nNumberCount; ++i) {
		fResult += fArray[i];
	}
}

inline void Sub() {
	for (unsigned int i = 0; i < nNumberCount; ++i) {
		fResult -= fArray[i];
	}
}

int main(void) {

	fArray = new float[nNumberCount];
	iArray = new int[nNumberCount];

	for (int i = 0; i < nNumberCount; ++i)
	{
		fArray[i] = 1.000001f;
		iArray[i] = 3;
	}

	Log("Init : %f %d", fArray[0], iArray[0]);

	// ************************* 乘法 *************************
	BENCHMARK(Test, Mul());
	Log("Result : %f", fResult);			// 这里要输出一次fResult，否则编译器的优化会删除执行运算的代码

	system("pause");

	// ************************* 乘法 *************************
	BENCHMARK(Multiple, Mul());
	Log("Result : %f", fResult);			// 这里要输出一次fResult，否则编译器的优化会删除执行运算的代码

	// ************************* 除法 *************************
	fResult = 1000.0f;

	BENCHMARK(Division, Div());
	Log("Result : %f", fResult);			// 这里要输出一次fResult，否则编译器的优化会删除执行运算的代码


	// ************************* 加法 *************************
	fResult = 1000.0f;

	BENCHMARK(Addition, Add());
	Log("Result : %f", fResult);			// 这里要输出一次fResult，否则编译器的优化会删除执行运算的代码

	// ************************* 减法 *************************
	fResult = 1000.0f;

	BENCHMARK(Subtraction, Sub());
	Log("Result : %f", fResult);			// 这里要输出一次fResult，否则编译器的优化会删除执行运算的代码

	BENCHMARK(IntMul, IntMul());
	Log("Result : %d", nResult);			// 这里要输出一次fResult，否则编译器的优化会删除执行运算的代码

	system("pause");

	return 0;
}