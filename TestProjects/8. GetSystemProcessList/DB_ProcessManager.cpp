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
		Log("Error: ��ȡȨ�ޱ������ʧ�ܣ��޷����ʽ�����Ϣ");

		return FALSE;
	}
	LUID luid;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		Log("Error: �Ҳ������ʽ���Ȩ�ޣ��޷����ʽ�����Ϣ");

		return FALSE;
	}
	TOKEN_PRIVILEGES tkp;
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = (bEnablePrivilege) ? SE_PRIVILEGE_ENABLED : FALSE;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {
		Log("Error: ��ȡ���ʽ���Ȩ��ʧ�ܣ��޷����ʽ�����Ϣ");

		return FALSE;
	}

	return TRUE;
}

HRESULT ProcessManager::UpdateProcessInfoList() {
	BOOL			bResult = TRUE;
	HANDLE			hProcessSnapShot;
	PROCESSENTRY32	processEntry;	// ���������Ϣ�Ľṹ��

	memset(&processEntry, 0, sizeof (processEntry));
	processEntry.dwSize = sizeof (processEntry);

	hProcessSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);		// ��ȡ���̿��վ��
	if (hProcessSnapShot != INVALID_HANDLE_VALUE) {
		bResult = Process32First(hProcessSnapShot, &processEntry);

		m_listProcessInfo.clear();	// ���������Ϣ�б�

		while (bResult) {
			ProcessInfo processInfo;
			if (GetProcessInfo(processEntry, processInfo)) {
				m_listProcessInfo.push_back(processInfo);
			}

			bResult = Process32Next(hProcessSnapShot, &processEntry);
		}

		// ��ȡϵͳ�ڴ�ʹ����Ϣ
		GetSystemMemoryInfo();

		// ��ȡ�������̵�ר�ù�����ռ���ڴ�
		GetProcessMemoryUse();

		// ��ȡ����������ϵͳ��CPUռ����Ϣ(��Ҫ����������һ��ʱ��)
		GetProcessCpuUse();
	}
	CloseHandle(hProcessSnapShot);

	return RESULT_SUCCESS;
}

HRESULT ProcessManager::OutputProcessInfoList(const char* path) {
	FILE* pFileProcessInfoList = fopen(path, "wb");

	if (!pFileProcessInfoList) {
		Log("Error: Ŀ���ļ��޷��򿪣�д���ļ�ʧ�ܣ��ļ�·����%s", path);

		return RESULT_FAILED;
	}

	// ��ȡʱ��
	char cBuffer[128];
	time_t logTime = time(NULL);
	unsigned uLength = strftime(cBuffer, 128, "%Y-%m-%d %X", localtime(&logTime));

	string strTitle = "ʱ��,";
	strTitle.append(cBuffer);
	fwrite(strTitle.c_str(), sizeof(char), strTitle.size(), pFileProcessInfoList);
	fwrite("\n", sizeof(char), 1, pFileProcessInfoList);

	//float fMemoryUseRate = m_sizePhysicalMemoryUsed * 100.0f / m_sizePhysicalMemoryTotal;
	UINT64 u64OneMegaByte = 1024 * 1024;
	UINT64 u64MemoryUsed = m_u64PhysicalMemoryUsed / u64OneMegaByte;
	UINT64 u64MemoryTotal = m_u64PhysicalMemoryTotal / u64OneMegaByte;
	fprintf(pFileProcessInfoList, "������,%d,,CPUʹ����,%d%%,,�����ڴ�,%d%%,%llu MB/%llu MB\n\n",
		m_dwProcessCount, m_dwCpuUsePercentage, m_dwMemoryUsePercentage, u64MemoryUsed, u64MemoryTotal);

	fflush(pFileProcessInfoList);

	fprintf(pFileProcessInfoList, "��������,����ID,CPUռ����,�ڴ�ռ��\n");

	// ���������Ϣ
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

	// ���Ի�ȡ���̾��
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, TRUE, processInfo.dwProcessID);

	if (hProcess == INVALID_HANDLE_VALUE) {
		Log("Error: ��ȡ���̾��ʧ�ܣ���������\"%s\", ����ID��%d", processInfo.strName.c_str(), processInfo.dwProcessID);

		return FALSE;
	}

	processInfo.hProcess = hProcess;

	// �޷�ֱ�ӻ�ȡ���̵��ڴ�ռ����Ϣ��ͨ��������ʽ���㣬�˴�ע�͵�
	//PROCESS_MEMORY_COUNTERS processMemoryCounters;
	//BOOL bResult = GetProcessMemoryInfo(hProcess, &processMemoryCounters, sizeof (processMemoryCounters));
	//if (!bResult) {
	//	Log("Error: ��ȡ�����ڴ�ռ����Ϣʧ�ܣ���������\"%s\", ����ID��%d", 
	//		processInfo.strName.c_str(), processInfo.dwProcessID);

	//	return FALSE;
	//}
	//// WorkingSetSize����������С�� = ר�ù�������С + ����������С
	//// �������������ʾ�Ľ���ռ���ڴ��СΪר�ù�������С
	//// PROCESS_MEMORY_COUNTERS�е������ڴ��С����bytesΪ��λ
	//processInfo.sizeMemoryUse = processMemoryCounters.PagefileUsage;

	return TRUE;
}

void ProcessManager::GetProcessCpuUse() {
	// ��ȡϵͳʱ��
	FILETIME m_ftPreSystemIdleTime;
	FILETIME m_ftPreSystemKernelTime;
	FILETIME m_ftPreSystemUserTime;
	GetSystemTimes(&m_ftPreSystemIdleTime, &m_ftPreSystemKernelTime, &m_ftPreSystemUserTime);

	// �����������ò�����ֻ�����������������ò�������ʡ�ԣ�
	FILETIME ftProcessCreation;
	FILETIME ftProcessExit;

	list<ProcessInfo>::iterator itProcessInfo = m_listProcessInfo.begin();
	while (itProcessInfo != m_listProcessInfo.end()) {
		if (!GetProcessTimes(itProcessInfo->hProcess, &ftProcessCreation, &ftProcessExit, 
					&(itProcessInfo->ftPreKernelTime), &(itProcessInfo->ftPreUserTime))) {
			Log("Error: ��ȡ����ʱ��ʧ�ܣ���������\"%s\", ����ID��%d", 
				itProcessInfo->strName.c_str(), itProcessInfo->dwProcessID);

			// ������ʧ�ܵĽ��̴��б����Ƴ�
			itProcessInfo = m_listProcessInfo.erase(itProcessInfo);
		}
		else {
			++itProcessInfo;
		}
	}

	// ����һ��ʱ��
	Sleep(500);

	FILETIME m_ftSystemIdleTime;
	FILETIME m_ftSystemKernelTime;
	FILETIME m_ftSystemUserTime;
	GetSystemTimes(&m_ftSystemIdleTime, &m_ftSystemKernelTime, &m_ftSystemUserTime);

	// �������ʱ��Ĳ�ֵ
	// CompareFileTime�᷵�ز���2��ȥ����1��ֵ
	UINT64 nSystemIdleTime = SubTime(m_ftPreSystemIdleTime, m_ftSystemIdleTime);
	UINT64 nSystemKernelTime = SubTime(m_ftPreSystemKernelTime, m_ftSystemKernelTime);
	UINT64 nSystemUserTime = SubTime(m_ftPreSystemUserTime, m_ftSystemUserTime);
	UINT64 nTotalSystemTime = nSystemKernelTime + nSystemUserTime;

	// CPU��ռ���� = ��ϵͳ�ں�ʱ�� + ϵͳ�û�ʱ�� - ϵͳ����ʱ�䣩 / ��ϵͳ�ں�ʱ�� + ϵͳ�û�ʱ�䣩* 100%
	m_dwCpuUsePercentage = (nTotalSystemTime - nSystemIdleTime) * 100 / nTotalSystemTime;

	// ����������̵�CPUռ����
	FILETIME m_ftProcessKernelTime;
	FILETIME m_ftProcessUserTime;

	itProcessInfo = m_listProcessInfo.begin();
	while (itProcessInfo != m_listProcessInfo.end()) {
		if (!GetProcessTimes(itProcessInfo->hProcess, &ftProcessCreation, &ftProcessExit,
					&m_ftProcessKernelTime, &m_ftProcessUserTime)) {
			Log("Error: ��ȡ����ʱ��ʧ�ܣ���������\"%s\", ����ID��%d",
				itProcessInfo->strName.c_str(), itProcessInfo->dwProcessID);

			// ������ʧ�ܵĽ��̴��б����Ƴ�
			itProcessInfo = m_listProcessInfo.erase(itProcessInfo);
		}
		else {
			// �������ʱ��Ĳ�ֵ
			// CompareFileTime�᷵�ز���2��ȥ����1��ֵ
			UINT64 nProcessKernelTime = SubTime(itProcessInfo->ftPreKernelTime, m_ftProcessKernelTime);
			UINT64 nProcessUserTime = SubTime(itProcessInfo->ftPreUserTime, m_ftProcessUserTime);

			itProcessInfo->dwCpuUsePercentage = (nProcessKernelTime + nProcessUserTime) * 100 / nTotalSystemTime;

			++itProcessInfo;
		}
	}
}

void ProcessManager::GetProcessMemoryUse() {
	// ��ȡ���̵�ר�ù�������С
	list<ProcessInfo>::iterator itProcessInfo = m_listProcessInfo.begin();
	while (itProcessInfo != m_listProcessInfo.end()) {
		HANDLE hProcess = itProcessInfo->hProcess;
		PSAPI_WORKING_SET_INFORMATION workingSetInfo;
		PSAPI_WORKING_SET_BLOCK *pWorkSetBlock = workingSetInfo.WorkingSetInfo;
		memset(&workingSetInfo, 0, sizeof(workingSetInfo));

		PBYTE pByte = NULL;

		// ��һ�ε��û�ȡʵ�ʻ�������С
		if (!QueryWorkingSet(hProcess, &workingSetInfo, sizeof(workingSetInfo))) {
			if (GetLastError() == ERROR_BAD_LENGTH) {
				DWORD realSize = sizeof(workingSetInfo.NumberOfEntries) + 
								 workingSetInfo.NumberOfEntries * sizeof(PSAPI_WORKING_SET_BLOCK);
				pByte = new BYTE[realSize];
				memset(pByte, 0, realSize);
				pWorkSetBlock = (PSAPI_WORKING_SET_BLOCK *)(pByte + sizeof(workingSetInfo.NumberOfEntries));
				
				// ���»�ȡ
				if (!QueryWorkingSet(hProcess, pByte, realSize))
				{
					delete[] pByte; // �����ڴ�

					Log("Error: ��ȡ�����ڴ�ռ����Ϣʧ�ܣ���������\"%s\", ����ID��%d",
						itProcessInfo->strName.c_str(), itProcessInfo->dwProcessID);

					++itProcessInfo;
					continue;
				}
			}
			else {
				Log("Error: ��ȡ�����ڴ�ռ����Ϣʧ�ܣ���������\"%s\", ����ID��%d",
					itProcessInfo->strName.c_str(), itProcessInfo->dwProcessID);

				++itProcessInfo;
				continue;
			}
		}

		UINT64 sizePrivateWorkingSet = 0;
		for (ULONG_PTR i = 0; i < workingSetInfo.NumberOfEntries; ++i)
		{
			// �����ǰҳ����ר�ù�����ҳ�棬����ר�ù�����ҳ���С
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
	// ��ȡϵͳ������Ϣ
	PERFORMANCE_INFORMATION performanceInfo;
	if (!GetPerformanceInfo(&performanceInfo, sizeof (performanceInfo))) {
		Log("Error: ��ȡϵͳ�ڴ�ʹ����Ϣʧ�� - 1");
	}

	// ��ȡϵͳ�еĽ�����Ŀ
	m_dwProcessCount = performanceInfo.ProcessCount;

	// PERFORMANCE_INFORMATION�д�����ڴ涼����pageΪ��λ����Ҫ����PageSize����ȡbytesΪ��λ�Ĵ�С
	// ͨ������ṹ���ȡ�������ڴ�������С������������е���ֵ�����ϣ���ΪSIZE_T��unsigned long���ͣ��޷���Ŵ���2^32���ڴ�����
	m_dwPageSize = performanceInfo.PageSize;
	//m_sizePhysicalMemoryTotal = performanceInfo.PhysicalTotal * m_sizePage;
	//m_sizePhysicalMemoryUsed = m_sizePhysicalMemoryTotal - performanceInfo.PhysicalAvailable * m_sizePage;

	// ʹ��������ʽ��ȡϵͳ�������ڴ��С
	MEMORYSTATUSEX memoryStatus;
	memoryStatus.dwLength = sizeof(memoryStatus);
	
	if (!GlobalMemoryStatusEx(&memoryStatus)) {
		Log("Error: ��ȡϵͳ�ڴ�ʹ����Ϣʧ�� - 2");
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