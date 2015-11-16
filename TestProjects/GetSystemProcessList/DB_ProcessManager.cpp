#define _CRT_SECURE_NO_WARNINGS

#include "DB_ProcessManager.h"
#include "../TestUtils/DB_Log.h"
#include <time.h>

using namespace DaBianYLK;
using namespace std;

ProcessManager::ProcessManager() {
	m_listProcessInfo.clear();
}

ProcessManager::~ProcessManager() {

}

BOOL ProcessManager::GetPrivilege() {
	HANDLE	hCurrentProcess	 = GetCurrentProcess();
	BOOL	bEnablePrivilege = TRUE;

	HANDLE hToken;
	if (!OpenProcessToken(hCurrentProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		Log("Error: 获取权限变更口令失败，无法访问进程信息");

		return FALSE;
	}
	LUID luid;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		Log("Error: 找不到访问进程权限，无法访问进程信息");

		return FALSE;
	}
	TOKEN_PRIVILEGES tkp;
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = (bEnablePrivilege) ? SE_PRIVILEGE_ENABLED : FALSE;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {
		Log("Error: 获取访问进程权限失败，无法访问进程信息");

		return FALSE;
	}

	return TRUE;
}

HRESULT ProcessManager::UpdateProcessInfoList() {
	BOOL			bResult = TRUE;
	HANDLE			hProcessSnapShot;
	PROCESSENTRY32	processEntry;	// 保存进程信息的结构体

	memset(&processEntry, 0, sizeof (processEntry));
	processEntry.dwSize = sizeof (processEntry);

	hProcessSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);		// 获取进程快照句柄
	if (hProcessSnapShot != INVALID_HANDLE_VALUE) {
		bResult = Process32First(hProcessSnapShot, &processEntry);

		m_listProcessInfo.clear();	// 情况进程信息列表

		while (bResult) {
			ProcessInfo processInfo;
			if (GetProcessInfo(processEntry, processInfo)) {
				m_listProcessInfo.push_back(processInfo);
			}

			bResult = Process32Next(hProcessSnapShot, &processEntry);
		}

		// 获取系统内存使用信息
		GetSystemMemoryInfo();

		// 获取各个进程的专用工作集占用内存
		GetProcessMemoryUse();

		// 获取各个进程与系统的CPU占用信息(需要将进程阻塞一段时间)
		GetProcessCpuUse();
	}
	CloseHandle(hProcessSnapShot);

	return RESULT_SUCCESS;
}

HRESULT ProcessManager::OutputProcessInfoList(const char* path) {
	FILE* pFileProcessInfoList = fopen(path, "wb");

	if (!pFileProcessInfoList) {
		Log("Error: 目标文件无法打开，写入文件失败，文件路径：%s", path);

		return RESULT_FAILED;
	}

	// 获取时间
	char cBuffer[128];
	time_t logTime = time(NULL);
	unsigned uLength = strftime(cBuffer, 128, "%Y-%m-%d %X", localtime(&logTime));

	string strTitle = "时间,";
	strTitle.append(cBuffer);
	fwrite(strTitle.c_str(), sizeof(char), strTitle.size(), pFileProcessInfoList);
	fwrite("\n", sizeof(char), 1, pFileProcessInfoList);

	//float fMemoryUseRate = m_sizePhysicalMemoryUsed * 100.0f / m_sizePhysicalMemoryTotal;
	UINT64 u64OneMegaByte = 1024 * 1024;
	UINT64 u64MemoryUsed = m_u64PhysicalMemoryUsed / u64OneMegaByte;
	UINT64 u64MemoryTotal = m_u64PhysicalMemoryTotal / u64OneMegaByte;
	fprintf(pFileProcessInfoList, "进程数,%d,,CPU使用率,%d%%,,物理内存,%d%%,%llu MB/%llu MB\n\n",
		m_dwProcessCount, m_dwCpuUsePercentage, m_dwMemoryUsePercentage, u64MemoryUsed, u64MemoryTotal);

	fflush(pFileProcessInfoList);

	fprintf(pFileProcessInfoList, "进程名称,进程ID,CPU占用率,内存占用\n");

	// 输出进程信息
	list<ProcessInfo>::iterator itProcessInfo = m_listProcessInfo.begin();
	while (itProcessInfo != m_listProcessInfo.end()) {
		UINT64 uMemoryUse = itProcessInfo->u64MemoryUse / 1024;
		fprintf(pFileProcessInfoList, "%s,%d,%d%%,%llu KB\n",
			itProcessInfo->strName.c_str(), itProcessInfo->dwProcessID, itProcessInfo->dwCpuUsePercentage, uMemoryUse);

		fflush(pFileProcessInfoList);

		++itProcessInfo;
	}

	fclose(pFileProcessInfoList);

	return RESULT_SUCCESS;
}

BOOL ProcessManager::GetProcessInfo(const PROCESSENTRY32 &processEntry, ProcessInfo &processInfo) {
	ZeroMemory(&processInfo, sizeof (processInfo));
	processInfo.strName		= processEntry.szExeFile;
	processInfo.dwProcessID = processEntry.th32ProcessID;

	// 尝试获取进程句柄
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, TRUE, processInfo.dwProcessID);

	if (hProcess == INVALID_HANDLE_VALUE) {
		Log("Error: 获取进程句柄失败，进程名：\"%s\", 进程ID：%d", processInfo.strName.c_str(), processInfo.dwProcessID);

		return FALSE;
	}

	processInfo.hProcess = hProcess;

	// 无法直接获取进程的内存占用信息，通过其他方式计算，此处注释掉
	//PROCESS_MEMORY_COUNTERS processMemoryCounters;
	//BOOL bResult = GetProcessMemoryInfo(hProcess, &processMemoryCounters, sizeof (processMemoryCounters));
	//if (!bResult) {
	//	Log("Error: 获取进程内存占用信息失败，进程名：\"%s\", 进程ID：%d", 
	//		processInfo.strName.c_str(), processInfo.dwProcessID);

	//	return FALSE;
	//}
	//// WorkingSetSize（工作集大小） = 专用工作集大小 + 共享工作集大小
	//// 任务管理器中显示的进程占用内存大小为专用工作集大小
	//// PROCESS_MEMORY_COUNTERS中的所有内存大小都以bytes为单位
	//processInfo.sizeMemoryUse = processMemoryCounters.PagefileUsage;

	return TRUE;
}

void ProcessManager::GetProcessCpuUse() {
	// 获取系统时间
	FILETIME m_ftPreSystemIdleTime;
	FILETIME m_ftPreSystemKernelTime;
	FILETIME m_ftPreSystemUserTime;
	GetSystemTimes(&m_ftPreSystemIdleTime, &m_ftPreSystemKernelTime, &m_ftPreSystemUserTime);

	// 这两个变量用不到，只是用来当参数（引用参数不能省略）
	FILETIME ftProcessCreation;
	FILETIME ftProcessExit;

	list<ProcessInfo>::iterator itProcessInfo = m_listProcessInfo.begin();
	while (itProcessInfo != m_listProcessInfo.end()) {
		if (!GetProcessTimes(itProcessInfo->hProcess, &ftProcessCreation, &ftProcessExit, 
					&(itProcessInfo->ftPreKernelTime), &(itProcessInfo->ftPreUserTime))) {
			Log("Error: 获取进程时间失败，进程名：\"%s\", 进程ID：%d", 
				itProcessInfo->strName.c_str(), itProcessInfo->dwProcessID);

			// 将访问失败的进程从列表中移除
			itProcessInfo = m_listProcessInfo.erase(itProcessInfo);
		}
		else {
			++itProcessInfo;
		}
	}

	// 阻塞一段时间
	Sleep(500);

	FILETIME m_ftSystemIdleTime;
	FILETIME m_ftSystemKernelTime;
	FILETIME m_ftSystemUserTime;
	GetSystemTimes(&m_ftSystemIdleTime, &m_ftSystemKernelTime, &m_ftSystemUserTime);

	// 计算各项时间的差值
	// CompareFileTime会返回参数2减去参数1的值
	UINT64 nSystemIdleTime = SubTime(m_ftPreSystemIdleTime, m_ftSystemIdleTime);
	UINT64 nSystemKernelTime = SubTime(m_ftPreSystemKernelTime, m_ftSystemKernelTime);
	UINT64 nSystemUserTime = SubTime(m_ftPreSystemUserTime, m_ftSystemUserTime);
	UINT64 nTotalSystemTime = nSystemKernelTime + nSystemUserTime;

	// CPU总占用率 = （系统内核时间 + 系统用户时间 - 系统空闲时间） / （系统内核时间 + 系统用户时间）* 100%
	m_dwCpuUsePercentage = (nTotalSystemTime - nSystemIdleTime) * 100 / nTotalSystemTime;

	// 计算各个进程的CPU占用率
	FILETIME m_ftProcessKernelTime;
	FILETIME m_ftProcessUserTime;

	itProcessInfo = m_listProcessInfo.begin();
	while (itProcessInfo != m_listProcessInfo.end()) {
		if (!GetProcessTimes(itProcessInfo->hProcess, &ftProcessCreation, &ftProcessExit,
					&m_ftProcessKernelTime, &m_ftProcessUserTime)) {
			Log("Error: 获取进程时间失败，进程名：\"%s\", 进程ID：%d",
				itProcessInfo->strName.c_str(), itProcessInfo->dwProcessID);

			// 将访问失败的进程从列表中移除
			itProcessInfo = m_listProcessInfo.erase(itProcessInfo);
		}
		else {
			// 计算各项时间的差值
			// CompareFileTime会返回参数2减去参数1的值
			UINT64 nProcessKernelTime = SubTime(itProcessInfo->ftPreKernelTime, m_ftProcessKernelTime);
			UINT64 nProcessUserTime = SubTime(itProcessInfo->ftPreUserTime, m_ftProcessUserTime);

			itProcessInfo->dwCpuUsePercentage = (nProcessKernelTime + nProcessUserTime) * 100 / nTotalSystemTime;

			++itProcessInfo;
		}
	}
}

void ProcessManager::GetProcessMemoryUse() {
	// 获取进程的专用工作集大小
	list<ProcessInfo>::iterator itProcessInfo = m_listProcessInfo.begin();
	while (itProcessInfo != m_listProcessInfo.end()) {
		HANDLE hProcess = itProcessInfo->hProcess;
		PSAPI_WORKING_SET_INFORMATION workingSetInfo;
		PSAPI_WORKING_SET_BLOCK *pWorkSetBlock = workingSetInfo.WorkingSetInfo;
		memset(&workingSetInfo, 0, sizeof(workingSetInfo));

		PBYTE pByte = NULL;

		// 第一次调用获取实际缓冲区大小
		if (!QueryWorkingSet(hProcess, &workingSetInfo, sizeof(workingSetInfo))) {
			if (GetLastError() == ERROR_BAD_LENGTH) {
				DWORD realSize = sizeof(workingSetInfo.NumberOfEntries) + 
								 workingSetInfo.NumberOfEntries * sizeof(PSAPI_WORKING_SET_BLOCK);
				pByte = new BYTE[realSize];
				memset(pByte, 0, realSize);
				pWorkSetBlock = (PSAPI_WORKING_SET_BLOCK *)(pByte + sizeof(workingSetInfo.NumberOfEntries));
				
				// 重新获取
				if (!QueryWorkingSet(hProcess, pByte, realSize))
				{
					delete[] pByte; // 清理内存

					Log("Error: 获取进程内存占用信息失败，进程名：\"%s\", 进程ID：%d",
						itProcessInfo->strName.c_str(), itProcessInfo->dwProcessID);

					++itProcessInfo;
					continue;
				}
			}
			else {
				Log("Error: 获取进程内存占用信息失败，进程名：\"%s\", 进程ID：%d",
					itProcessInfo->strName.c_str(), itProcessInfo->dwProcessID);

				++itProcessInfo;
				continue;
			}
		}

		UINT64 sizePrivateWorkingSet = 0;
		for (ULONG_PTR i = 0; i < workingSetInfo.NumberOfEntries; ++i)
		{
			// 如果当前页面是专用工作集页面，增加专用工作集页面大小
			if (!pWorkSetBlock[i].Shared) {
				sizePrivateWorkingSet += m_dwPageSize;
			}
		}

		itProcessInfo->u64MemoryUse = sizePrivateWorkingSet;

		if (pByte) {
			delete[] pByte;
		}
		
		++itProcessInfo;
	}
}

void ProcessManager::GetSystemMemoryInfo() {
	// 获取系统运行信息
	PERFORMANCE_INFORMATION performanceInfo;
	if (!GetPerformanceInfo(&performanceInfo, sizeof (performanceInfo))) {
		Log("Error: 获取系统内存使用信息失败 - 1");
	}

	// 获取系统中的进程数目
	m_dwProcessCount = performanceInfo.ProcessCount;

	// PERFORMANCE_INFORMATION中储存的内存都是以page为单位，需要乘以PageSize来获取bytes为单位的大小
	// 通过这个结构体获取的物理内存总量大小与任务管理器中的数值不符合，因为SIZE_T是unsigned long类型，无法存放大于2^32的内存数量
	m_dwPageSize = performanceInfo.PageSize;
	//m_sizePhysicalMemoryTotal = performanceInfo.PhysicalTotal * m_sizePage;
	//m_sizePhysicalMemoryUsed = m_sizePhysicalMemoryTotal - performanceInfo.PhysicalAvailable * m_sizePage;

	// 使用其他方式获取系统的物理内存大小
	MEMORYSTATUSEX memoryStatus;
	memoryStatus.dwLength = sizeof(memoryStatus);
	
	if (!GlobalMemoryStatusEx(&memoryStatus)) {
		Log("Error: 获取系统内存使用信息失败 - 2");
	}

	m_u64PhysicalMemoryTotal = memoryStatus.ullTotalPhys;
	m_u64PhysicalMemoryUsed = m_u64PhysicalMemoryTotal - memoryStatus.ullAvailPhys;
	m_dwMemoryUsePercentage = memoryStatus.dwMemoryLoad;
}

UINT64 ProcessManager::SubTime(FILETIME a, FILETIME b) {
	LARGE_INTEGER largeIntA;
	LARGE_INTEGER largeIntB;

	largeIntA.LowPart = a.dwLowDateTime;
	largeIntA.HighPart = a.dwHighDateTime;

	largeIntB.LowPart = b.dwLowDateTime;
	largeIntB.HighPart = b.dwHighDateTime;

	return largeIntB.QuadPart - largeIntA.QuadPart;
}