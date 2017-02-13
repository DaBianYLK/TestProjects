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
	/********获取操作系统版本，Service pack版本、系统类型************/
	void GetOSVersion(string &strOSVersion, string &strServiceVersion);
	BOOL IsWow64();//判断是否为64位操作系统

	/***********获取网卡数目和名字***********/
	int  GetInterFaceCount();
	void GetInterFaceName(string &InterfaceName, int pNum);

	/***获取物理内存和虚拟内存大小***/
	void GetMemoryInfo(string &dwTotalPhys, string &dwTotalVirtual);

	/****获取CPU名称、内核数目、主频*******/
	void GetCpuInfo(string &chProcessorName, string &chProcessorType, DWORD &dwNum, DWORD &dwMaxClockSpeed);

	/****获取硬盘信息****/
	void GetDiskInfo(DWORD &dwNum, string chDriveInfo[]);

	/****获取显卡信息*****/
	void GetDisplayCardInfo(DWORD &dwNum, string chCardName[]);
private:
	vector<string>		Interfaces;		                  //保存所有网卡的名字
	vector<DWORD>		Bandwidths;	  //各网卡的带宽
	vector<DWORD>		TotalTraffics;    //各网卡的总流量
};