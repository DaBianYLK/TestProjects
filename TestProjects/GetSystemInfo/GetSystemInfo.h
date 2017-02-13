#pragma once

#include <Windows.h>
#include <string>
#include <vector>

using namespace std;

class GetSysInfo
{
public:
	GetSysInfo(void);
	~GetSysInfo(void);

public:
	/********��ȡ����ϵͳ�汾��Service pack�汾��ϵͳ����************/
	void GetOSVersion(string &strOSVersion, string &strServiceVersion);
	BOOL IsWow64();//�ж��Ƿ�Ϊ64λ����ϵͳ

	/***********��ȡ������Ŀ������***********/
	int  GetInterFaceCount();
	void GetInterFaceName(string &InterfaceName, int pNum);

	/***��ȡ�����ڴ�������ڴ��С***/
	void GetMemoryInfo(string &dwTotalPhys, string &dwTotalVirtual);

	/****��ȡCPU���ơ��ں���Ŀ����Ƶ*******/
	void GetCpuInfo(string &chProcessorName, string &chProcessorType, DWORD &dwNum, DWORD &dwMaxClockSpeed);

	/****��ȡӲ����Ϣ****/
	void GetDiskInfo(DWORD &dwNum, string chDriveInfo[]);

	/****��ȡ�Կ���Ϣ*****/
	void GetDisplayCardInfo(DWORD &dwNum, string chCardName[]);
private:
	vector<string>		Interfaces;		                  //������������������
	vector<DWORD>		Bandwidths;	  //�������Ĵ���
	vector<DWORD>		TotalTraffics;    //��������������
};