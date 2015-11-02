#pragma comment(lib, "TestUtils.lib")

#include "../TestUtils/DB_Log.h"
#include "../TestUtils/DB_Timer.h"

#include <iostream>

using namespace std;
using namespace DaBianYLK;

/*
* 测试目的：判断编译器对整数除法的优化
*
* 测试方法：以2的整数次幂以及非2的整数次幂为除数运行除法运算各1亿次，比较消耗的时间
*
* Release模式下计时结果：
*	1.当result为unsigned int类型
*		除以4		0.030316s
*		移位2		0.030457s
*		除以16		0.030316s
*		移位4		0.030457s
*		除以5000	0.234306s
*		除以500		0.146523s
*
*	2.当result为int类型
*		除以4		0.117160s
*		移位2		0.030250s	（不考虑符号）
*		除以16		0.117160s
*		移位4		0.030076s	（不考虑符号）
*		除以5000	0.209628s
*		除以500		0.206247s
*		移位2		0.090318s	（考虑符号）
*
* 结果分析：
*		1. 除以2的整数次幂会自动优化为移位
*		2. 无符号整型除以某些特殊数字时消耗的时间会增加50%，原因不明
*		3. 当运算类型为有符号整型时，编译器的优化效率会降低
*		4. 无符号整型运算中，移位与除法的时间消耗比约为 1 : 5
*		5. 有符号整型运算中，移位与除法的时间消耗比约为 1 : 7 （不考虑符号时）
*		6. 有符号整型运算中，采用自定义的移位优化效率比编译器的自动优化高20%到25% （考虑符号）
*		7. 有符号整型运算中，考虑符号的移位与不考虑符号的移位时间消耗比为 1 ： 3
*
* 结论：
*		在希望效率最大化的情况下，无符号整型运算可以不进行手动移位优化，有符号整型运算需要进行手动移位优化
*/

int main(void) {
	unsigned int nCalculation = 99999999;
	//unsigned int result = 1111111;
	int result = -99999999;

	BENCHMARK_START(
		DivisionWithOptimization,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = result / 4;
		}
	);

	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Divide by 4 \t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(DivisionWithOptimization), result);

	BENCHMARK_START(
		ShiftWith2,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = result >> 2;
		}
	);

	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Shifted by 2 \t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(ShiftWith2), result);

	BENCHMARK_START(
		DivisionWith16,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = result / 4;
		}
	);

	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Divide by 16 \t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(DivisionWithOptimization), result);

	BENCHMARK_START(
		ShiftWith4,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = result >> 4;
		}
	);

	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Shifted by 4 \t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(ShiftWith4), result);

	BENCHMARK_START(
		DivisionWith50000,
		for (unsigned int i = 0; i < nCalculation; ++i) {
			result = result / 50000;
		}
	);

	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Divide by 50000\t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(DivisionWith50000), result);

	BENCHMARK_START(
		DivisionWith5000,
	for (unsigned int i = 0; i < nCalculation; ++i) {
		result = result / 5000;
	}
	);

	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Divide by 5000 \t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(DivisionWith5000), result);

	BENCHMARK_START(
		NegativeShiftWith2,
	for (unsigned int i = 0; i < nCalculation; ++i) {
		bool negtive = false;
		if (result <= 0) {
			result = -result;
			negtive = true;
		}

		result = result >> 2;

		if (negtive) {
			result = -result;
		}
	}
	);

	// 这里要输出一次result,否则编译器的优化会删除执行运算的代码
	Log("Neg Shift by 2\t - Past Time : %fs, Result : %f", BENCHMARK_RESULT(NegativeShiftWith2), result);

	system("pause");

	return 0;
}