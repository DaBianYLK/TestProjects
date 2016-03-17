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
* 测试目的：判断加法与或运算的效率
*
* 结果分析：
*		1. Release模式下各运行1亿次，加法耗时0.031s左右，或运算耗时0.029s左右
*		2. Debug模式下各运行1亿次，加法耗时0.226s左右，或运算耗时0.217s左右
*
* 结论：
*		或运算效率略高于加法运算。
*/

const unsigned int nCalculation = 100000000;
unsigned long long resultNumber = 0;
unsigned int addNumber = 16;

inline void AddOperation() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		resultNumber += addNumber;
	}
}

inline void OrOperation() {
	for (unsigned int i = 0; i < nCalculation; ++i) {
		resultNumber |= addNumber;
	}
}

int main(void) {

	// ************************* 加法 *************************
	BENCHMARK(AddOperation, AddOperation());
	Log("Result : %ulld", resultNumber);			// 这里要输出一次result,否则编译器的优化会删除执行运算的代码

	// ************************* 或运算 *************************
	resultNumber = 0;
	addNumber = 16;

	BENCHMARK(OrOperation, OrOperation());
	Log("Result : %ulld", resultNumber);			// 这里要输出一次result,否则编译器的优化会删除执行运算的代码

	system("pause");

	return 0;
}