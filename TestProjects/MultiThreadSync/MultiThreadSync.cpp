// Windows下多线程同步的demo

#include <iostream>
#include <Windows.h>

using namespace std;

enum ESyncMethod
{
	ESM_CriticalSection,
	ESM_Mutex,
	ESM_Semaphore,
	ESM_Event
};

struct CriticalSection
{
	CRITICAL_SECTION section;

	CriticalSection()		{ InitializeCriticalSection(&section); }
	~CriticalSection()		{ DeleteCriticalSection(&section); }
	void Enter()			{ EnterCriticalSection(&section); }
	void Leave()			{ LeaveCriticalSection(&section); }
};

struct Mutex
{
	Mutex() {}
	~Mutex() {}
	void Lock() {}
	void Unlock() {}
};

struct Semaphore
{
	HANDLE hSemaphore;

	Semaphore()		{ hSemaphore = CreateSemaphore(nullptr, 1, 1, nullptr); }
	~Semaphore()	{ CloseHandle(hSemaphore); }
	void P()		{ WaitForSingleObject(hSemaphore, INFINITE); }		// Proberen，荷兰语“测试”，等待直到信号量不为0，使信号量减一并返回
	void V()		{ ReleaseSemaphore(hSemaphore, 1, nullptr); }		// Verhogen，荷兰语“增加”，使信号量加一
};

struct Event
{
	Event() {}
	~Event() {}
	void Set() {}
	void Reset() {}
	void Wait() {}
};

const int			nThreadCount	= 3;
int					nCounter		= 0;
ESyncMethod			syncMethod		= ESM_Semaphore;

CriticalSection		section;
Mutex				mutex;
Semaphore			semaphore;
Event				event;

unsigned long CriticalSectionThreadFunc(void* pParam)
{
	for (int i = 0; i < 10; ++i)
	{
		section.Enter();
		nCounter++;
		cout << "Critical Section Thread ID: " << reinterpret_cast<int>(pParam) << ", \t Counter: " << nCounter << endl;
		section.Leave();
	}
	return 0;
};

unsigned long MutexThreadFunc(void* pParam)
{
	for (int i = 0; i < 10; ++i)
	{
		mutex.Lock();
		nCounter++;
		cout << "Mutex Section Thread ID: " << reinterpret_cast<int>(pParam) << ", \t Counter: " << nCounter << endl;
		mutex.Unlock();
	}
	return 0;
};

unsigned long SemaphoreThreadFunc(void* pParam)
{
	for (int i = 0; i < 10; ++i)
	{
		semaphore.P();
		nCounter++;
		cout << "Semaphore Thread ID: " << reinterpret_cast<int>(pParam) << ", \t Counter: " << nCounter << endl;
		semaphore.V();
	}
	return 0;
};

unsigned long EventThreadFunc(void* pParam)
{
	for (int i = 0; i < 10; ++i)
	{
		event.Wait();
		event.Reset();
		nCounter++;
		cout << "Event Section Thread ID: " << reinterpret_cast<int>(pParam) << ", \t Counter: " << nCounter << endl;
		event.Set();
	}
	return 0;
};

void StartThread(LPTHREAD_START_ROUTINE pThreadFunc)
{
	for (int i = 0; i < nThreadCount; ++i)
	{
		CreateThread(nullptr, 0, pThreadFunc, reinterpret_cast<void*>(i), 0, nullptr);
	}
}

int main(void)
{
	switch (syncMethod)
	{
	case ESM_CriticalSection:
		StartThread(reinterpret_cast<LPTHREAD_START_ROUTINE>(CriticalSectionThreadFunc));
		break;

	case ESM_Mutex:
		break;

	case ESM_Semaphore:
		StartThread(reinterpret_cast<LPTHREAD_START_ROUTINE>(SemaphoreThreadFunc));
		break;

	case ESM_Event:
		break;

	default:
		break;
	}

	Sleep(1000);
	system("pause");
	return 0;
}