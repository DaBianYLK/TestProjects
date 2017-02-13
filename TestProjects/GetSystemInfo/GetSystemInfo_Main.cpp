#include <iostream>
#include "GetSystemInfo.h"

using namespace std;

int main()
{
	GetSysInfo sysInfo;

	string strOSVersion;
	string strServiecVersion;
	sysInfo.GetOSVersion(strOSVersion, strServiecVersion);
	cout << strOSVersion << endl;
	cout << strServiecVersion << endl;

	cout << (bool)sysInfo.IsWow64() << endl;

	int nInterafaceCount = sysInfo.GetInterFaceCount();
	cout << nInterafaceCount << endl;
	for (int i = 0; i < nInterafaceCount; ++i)
	{
		string strTemp;
		sysInfo.GetInterFaceName(strTemp, i);
		cout << "Interface[" << i << "]: " << strTemp << endl;
	}
	
	string strTotalPhys;
	string strTotalVirtual;
	sysInfo.GetMemoryInfo(strTotalPhys, strTotalVirtual);
	cout << strTotalPhys << endl;
	cout << strTotalVirtual << endl;

	string strProcesserName;
	string strProcesserType;
	DWORD dwCPUCoreNum;
	DWORD dwMaxClockSpeed;
	sysInfo.GetCpuInfo(strProcesserName, strProcesserType, dwCPUCoreNum, dwMaxClockSpeed);
	cout << strProcesserName << endl;
	cout << strProcesserType << endl;
	cout << dwCPUCoreNum << endl;
	cout << dwMaxClockSpeed << endl;
	
	DWORD dwDiskNum;
	string strDiskInfo[20];
	sysInfo.GetDiskInfo(dwDiskNum, strDiskInfo);
	for (unsigned i = 0; i < dwDiskNum; ++i)
	{
		cout << "Disk[" << i << "]: " << strDiskInfo[i] << endl;
	}

	DWORD dwVideoCardNum;
	string strVideoCardInfo[20];
	sysInfo.GetDisplayCardInfo(dwVideoCardNum, strVideoCardInfo);
	for (unsigned i = 0; i < dwVideoCardNum; ++i)
	{
		cout << "VideoCard[" << i << "]: " << strVideoCardInfo[i] << endl;
	}

	system("pause");

	return 0;
}