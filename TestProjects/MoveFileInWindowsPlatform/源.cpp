#ifdef _DEBUG
#pragma comment(lib, "../Debug/TestUtils.lib")
#else
#pragma comment(lib, "../Release/TestUtils.lib")
#endif

#include "../TestUtils/DBLog.h"

using namespace DaBianYLK;

#include <iostream>
#include <windows.h>
#include <cstdio>
#include <atlstr.h>

// AccessFile操作所需要的头文件
#include <io.h>

using namespace std; 

// 重新定义_access函数名，更易于理解与使用
// _access：检查文件的访问权限
// 参数：	1. 文件路径
//			2. 文件访问模式
// 返回值： 若文件存在且具有相应权限，返回0；若文件不存在或没有对应的访问权限，返回-1；
#define AccessFile(FilePath, AccessMode) _access(FilePath, AccessMode)

// _access函数的访问模式（第二个参数）
#define AccessFile_Exist	0	
#define AccessFile_Write	2
#define AccessFile_Read		4


string sourcePath = "E:/FileOperationTest/Source/";
string destinationPath = "E:/FileOperationTest/Destination/";
string fileNameA = "TestFile1.txt";
string fileNameB = "TestFile2.txt";

string copySource = sourcePath + fileNameA;
string copyDestination = destinationPath + fileNameA;

string moveSource = sourcePath + fileNameB;
string moveDestination = destinationPath + fileNameB;


void MoveFileByCopyAndDeleteFunction() {
	Log("Way 1 - Move file by \"CopyFile\" and \"DeleteFile\" function.");

	// 查找文件是否存在且是否具有读写权限
	if (AccessFile(copySource.c_str(), AccessFile_Write | AccessFile_Read) == 0) {
		// 检查目标路径是否存在，如果不存在则创建
		if (AccessFile(destinationPath.c_str(), AccessFile_Exist) == -1) {
			Log("\tDestination directory is not exist, program will create it.");

			// 参数：1. 目录路径
			//		 2. 目录的安全描述，一般置为NULL即可
			CreateDirectory(destinationPath.c_str(), NULL);

			Log("\tDestination directory create successfully.");
		}

		Log("\tStart to copy file.");
		// ======== 移动文件函数 ========
		// 参数：	1. 源文件路径
		//			2. 目标文件路径
		//			3. 若文件存在是否终止操作（若此参数为FALSE，则会覆盖已经存在的文件）
		if (CopyFile(copySource.c_str(), copyDestination.c_str(), TRUE)) {
			Log("\tCopy file successfully.");

			Log("\tStart to delete file.");
			DeleteFile(copySource.c_str());
			Log("\tDelete file successfully.");
		}
		else {
			Log("\tErorr : Copy file failed. Please check if the file is already exist.");
		}
	}
	else {
		Log("\tErorr : Source File is not accessible.");
	}

	cout << endl;
}

void MoveFileByMoveFunction() {
	Log("Way 2 - Move file by \"MoveFile\" function.");

	// 查找文件是否存在且是否具有读写权限
	if (AccessFile(moveSource.c_str(), AccessFile_Write | AccessFile_Read) == 0) {
		// 检查目标路径是否存在，如果不存在则创建
		if (AccessFile(destinationPath.c_str(), AccessFile_Exist) == -1) {
			Log("\tDestination directory is not exist, program will create it.");

			// 参数：1. 目录路径
			//		 2. 目录的安全描述，一般置为NULL即可
			CreateDirectory(destinationPath.c_str(), NULL);

			Log("\tDestination directory create successfully.");
		}

		Log("\tStart to move file.");

		if (MoveFile(moveSource.c_str(), moveDestination.c_str())) {
			Log("\tMove file successfully.");
		}
		else {
			Log("\tErorr : Move file failed. Please check if the file is already exist.");
		}

	}
	else {
		Log("\tErorr : Source File is not accessible.");
	}

	cout << endl;
}

// 获取当前目录中的文件列表并打印
// 参数： 1. 需要显示的目录  2. 目录缩进层级
void GetFileListInDirectory(string directoryPath, int indentLevel = 0) {
	string strIndent = "";
	for (int i = 0; i < indentLevel; ++i) {
		strIndent.append("\t");
	}

	WIN32_FIND_DATA fileData;
	LARGE_INTEGER fileSize;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	// Prepare string for use with FindFile functions. Append '/*' to the directory name.
	// Because thers is already a '/', so only append '*' here.
	string currentDirectory = directoryPath;
	currentDirectory.append("*");

	// Find the first file in the directory.
	hFind = FindFirstFile(currentDirectory.c_str(), &fileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		// 如果当前文件夹为空
		string ouputInfo = strIndent;
		Log(ouputInfo.append("[ Current directory is empty. ]").c_str());

		return;
	}

	// List all the files in the directory with some info about them.
	do {
		char* cBuffer = new char[1024];
		string ouputInfo = strIndent;

		// 如果当前文件是一个目录
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// 如果当前目录名为“.”（即当前目录）或“..”（即父目录），则继续查找目录中的其他文件
			if (strcmp(fileData.cFileName, ".") == 0 || strcmp(fileData.cFileName, "..") == 0) {
				continue;
			}

			sprintf(cBuffer, "<DIR> %s", fileData.cFileName);
			Log(ouputInfo.append(cBuffer).c_str());

			// 查找子目录
			string subDirectory = directoryPath;
			subDirectory.append(fileData.cFileName);
			subDirectory.append("/");
			GetFileListInDirectory(subDirectory, indentLevel + 1);
		}
		// 如果当前文件不是目录
		else {
			fileSize.LowPart = fileData.nFileSizeLow;
			fileSize.HighPart = fileData.nFileSizeHigh;

			sprintf(cBuffer, "%s\t\t%ld bytes", fileData.cFileName, fileSize.QuadPart);
			Log(ouputInfo.append(cBuffer).c_str());
		}

		delete[] cBuffer;
	} while (FindNextFile(hFind, &fileData) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES) {
		Log("Unknown Error has occur.");
	}

	FindClose(hFind);
}

void main()
{
	MoveFileByCopyAndDeleteFunction();

	MoveFileByMoveFunction();

	Log("Start to find files in the source directory.");
	GetFileListInDirectory(sourcePath);
	Log("File find Finished.");
	
	system("pause");

	return;
}