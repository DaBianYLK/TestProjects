#pragma once

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "TestUtils.lib")

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <assert.h>
#include <list>

#define RESULT_SUCCESS	0
#define RESULT_FAILED	-1

namespace DaBianYLK {
	struct ProcessInfo {
		std::string	strName;
		DWORD		dwProcessID;
		HANDLE		hProcess;
		DWORD		dwCpuUsePercentage;	// 进程占用的CPU资源百分比，在[0, 100]取值
		UINT64		u64MemoryUse;		// 进程占用的内存大小，单位bytes

		// 计算进程的CPU占用率用到的变量
		FILETIME	ftPreKernelTime;
		FILETIME	ftPreUserTime;
	};

	class ProcessManager {
	public:
		ProcessManager();
		~ProcessManager();

		BOOL GetPrivilege();				// 获取访问进程信息的权限
		HRESULT UpdateProcessInfoList();
		HRESULT OutputProcessInfoList(const char* path);

	private:
		BOOL GetProcessInfo(const PROCESSENTRY32 &processEntry, ProcessInfo &processInfo);
		void GetProcessCpuUse();
		void GetProcessMemoryUse();			// 获取进程的专用工作集占用大小（与Windows任务管理器一致）
		void GetSystemMemoryInfo();
		UINT64 SubTime(FILETIME a, FILETIME b);

	private:
		std::list<ProcessInfo> m_listProcessInfo;

		DWORD	m_dwPageSize;				// 内存分页大小，单位bytes
		UINT64	m_u64PhysicalMemoryUsed;	// 系统已用物理内存大小，单位bytes
		UINT64	m_u64PhysicalMemoryTotal;	// 系统总物理内存大小，单位bytes
		DWORD	m_dwMemoryUsePercentage;	// 系统中已用内存百分比，在[0, 100]取值
		DWORD	m_dwCpuUsePercentage;		// 系统中已用CPU百分比，在[0, 100]取值

		DWORD	m_dwProcessCount;			// 系统中运行的进程数
	};
}