#include <Windows.h>

namespace DaBianYLK {
	static LARGE_INTEGER nFrequency;		// 系统时钟频率
	static LARGE_INTEGER nStart;
	static LARGE_INTEGER nCurrent;

	void StartTimer();
	float GetPastSeconds();
}

// The "TaskName" can't be duplicate in a same file.
#define BENCHMARK_START(TaskName, TaskCode)	\
	float BenchmarkResult_##TaskName; 	\
	DaBianYLK::StartTimer();			\
	TaskCode;							\
	BenchmarkResult_##TaskName = DaBianYLK::GetPastSeconds();

// Get the time of performance of task in seconds.
#define BENCHMARK_RESULT(TaskName) BenchmarkResult_##TaskName	