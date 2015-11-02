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
		DWORD		dwCpuUsePercentage;	// ����ռ�õ�CPU��Դ�ٷֱȣ���[0, 100]ȡֵ
		UINT64		u64MemoryUse;		// ����ռ�õ��ڴ��С����λbytes

		// ������̵�CPUռ�����õ��ı���
		FILETIME	ftPreKernelTime;
		FILETIME	ftPreUserTime;
	};

	class ProcessManager {
	public:
		ProcessManager();
		~ProcessManager();

		BOOL GetPrivilege();				// ��ȡ���ʽ�����Ϣ��Ȩ��
		HRESULT UpdateProcessInfoList();
		HRESULT OutputProcessInfoList(const char* path);

	private:
		BOOL GetProcessInfo(const PROCESSENTRY32 &processEntry, ProcessInfo &processInfo);
		void GetProcessCpuUse();
		void GetProcessMemoryUse();			// ��ȡ���̵�ר�ù�����ռ�ô�С����Windows���������һ�£�
		void GetSystemMemoryInfo();
		UINT64 SubTime(FILETIME a, FILETIME b);

	private:
		std::list<ProcessInfo> m_listProcessInfo;

		DWORD	m_dwPageSize;				// �ڴ��ҳ��С����λbytes
		UINT64	m_u64PhysicalMemoryUsed;	// ϵͳ���������ڴ��С����λbytes
		UINT64	m_u64PhysicalMemoryTotal;	// ϵͳ�������ڴ��С����λbytes
		DWORD	m_dwMemoryUsePercentage;	// ϵͳ�������ڴ�ٷֱȣ���[0, 100]ȡֵ
		DWORD	m_dwCpuUsePercentage;		// ϵͳ������CPU�ٷֱȣ���[0, 100]ȡֵ

		DWORD	m_dwProcessCount;			// ϵͳ�����еĽ�����
	};
}