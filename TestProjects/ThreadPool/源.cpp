#pragma comment(lib, "TestUtils.lib")

#include "../TestUtils/DB_Log.h"
#include "../TestUtils/DB_Timer.h"

#include <iostream>

using namespace std;
using namespace DaBianYLK;


int iParamCount = 0;

struct ThreadParam {
	ThreadParam() {
		char cBuffer[32];
		sprintf_s(cBuffer, "Param - %d", iParamCount);
		strMessage = cBuffer;

		++iParamCount;
	}

	string strMessage;
};

const int iMaxThreadCount = 20;
ThreadParam param[iMaxThreadCount];

const int iCalculaionCount = 200000000;

volatile int iLock = 0;

void CALLBACK ThreadFunction(PTP_CALLBACK_INSTANCE pInstance, void* pParameter, PTP_WORK pWork)
{
	float fResult = 1.0f;
	float fFactor = 1.112f;

	for (int i = 0; i < iCalculaionCount; ++i) {
		fResult += fFactor;
	}
	//printf("Thread ID : %d,\nThread Message : %s\nResult : %f\n\n", GetCurrentThreadId(), ((ThreadParam*)pParameter)->strMessage.c_str(), fResult);
	printf("Thread : %s,\nThread Pool Result : %f\n", ((ThreadParam*)pParameter)->strMessage.c_str(), fResult);

	--iLock;
}

void ThreadPool() {
	PTP_POOL pool = NULL;
	TP_CALLBACK_ENVIRON CallBackEnviron;
	PTP_CLEANUP_GROUP cleanupgroup = NULL;

	InitializeThreadpoolEnvironment(&CallBackEnviron);

	// Create a custom, dedicated thread pool
	pool = CreateThreadpool(NULL);

	if (!pool) {
		return;
	}

	// The thread pool is made persistent simply by setting both the minimum and maximum threads to 1.
	SetThreadpoolThreadMaximum(pool, 3);
	SetThreadpoolThreadMinimum(pool, 3);

	// Create a cleanup group for this thread pool
	cleanupgroup = CreateThreadpoolCleanupGroup();

	if (!cleanupgroup) {
		CloseThreadpool(pool);
		return;
	}

	// Associate the callback environment with our thread pool
	SetThreadpoolCallbackPool(&CallBackEnviron, pool);

	for (int i = 0; i < iMaxThreadCount; ++i) {
		PTP_WORK work = CreateThreadpoolWork(ThreadFunction, &param[i], &CallBackEnviron);
		if (!work) {
			printf("Thread Work [%d] create failed.\n", i);
			return;
		}

		++iLock;
		SubmitThreadpoolWork(work);
	}

	while (iLock) {
		Sleep(5);
	}

	CloseThreadpoolCleanupGroupMembers(cleanupgroup, FALSE, NULL); 
	CloseThreadpoolCleanupGroup(cleanupgroup);
	CloseThreadpool(pool);
	return;
}

void SingleThread() {
	for (int i = 0; i < iMaxThreadCount; ++i) {
		float fResult = 1.0f;
		float fFactor = 1.112f;

		for (int j = 0; j < iCalculaionCount; ++j) {
			fResult += fFactor;
		}

		printf("Single Thread Result : %f\n", fResult);
	}
}

void main() {
	BENCHMARK(ThreadPool, ThreadPool());

	BENCHMARK(SingleThread, SingleThread());

	system("pause");

	return;
}