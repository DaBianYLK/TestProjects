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

// AccessFile��������Ҫ��ͷ�ļ�
#include <io.h>

using namespace std; 

// ���¶���_access�������������������ʹ��
// _access������ļ��ķ���Ȩ��
// ������	1. �ļ�·��
//			2. �ļ�����ģʽ
// ����ֵ�� ���ļ������Ҿ�����ӦȨ�ޣ�����0�����ļ������ڻ�û�ж�Ӧ�ķ���Ȩ�ޣ�����-1��
#define AccessFile(FilePath, AccessMode) _access(FilePath, AccessMode)

// _access�����ķ���ģʽ���ڶ���������
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

	// �����ļ��Ƿ�������Ƿ���ж�дȨ��
	if (AccessFile(copySource.c_str(), AccessFile_Write | AccessFile_Read) == 0) {
		// ���Ŀ��·���Ƿ���ڣ�����������򴴽�
		if (AccessFile(destinationPath.c_str(), AccessFile_Exist) == -1) {
			Log("\tDestination directory is not exist, program will create it.");

			// ������1. Ŀ¼·��
			//		 2. Ŀ¼�İ�ȫ������һ����ΪNULL����
			CreateDirectory(destinationPath.c_str(), NULL);

			Log("\tDestination directory create successfully.");
		}

		Log("\tStart to copy file.");
		// ======== �ƶ��ļ����� ========
		// ������	1. Դ�ļ�·��
		//			2. Ŀ���ļ�·��
		//			3. ���ļ������Ƿ���ֹ���������˲���ΪFALSE����Ḳ���Ѿ����ڵ��ļ���
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

	// �����ļ��Ƿ�������Ƿ���ж�дȨ��
	if (AccessFile(moveSource.c_str(), AccessFile_Write | AccessFile_Read) == 0) {
		// ���Ŀ��·���Ƿ���ڣ�����������򴴽�
		if (AccessFile(destinationPath.c_str(), AccessFile_Exist) == -1) {
			Log("\tDestination directory is not exist, program will create it.");

			// ������1. Ŀ¼·��
			//		 2. Ŀ¼�İ�ȫ������һ����ΪNULL����
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

// ��ȡ��ǰĿ¼�е��ļ��б���ӡ
// ������ 1. ��Ҫ��ʾ��Ŀ¼  2. Ŀ¼�����㼶
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
		// �����ǰ�ļ���Ϊ��
		string ouputInfo = strIndent;
		Log(ouputInfo.append("[ Current directory is empty. ]").c_str());

		return;
	}

	// List all the files in the directory with some info about them.
	do {
		char* cBuffer = new char[1024];
		string ouputInfo = strIndent;

		// �����ǰ�ļ���һ��Ŀ¼
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// �����ǰĿ¼��Ϊ��.��������ǰĿ¼����..��������Ŀ¼�������������Ŀ¼�е������ļ�
			if (strcmp(fileData.cFileName, ".") == 0 || strcmp(fileData.cFileName, "..") == 0) {
				continue;
			}

			sprintf(cBuffer, "<DIR> %s", fileData.cFileName);
			Log(ouputInfo.append(cBuffer).c_str());

			// ������Ŀ¼
			string subDirectory = directoryPath;
			subDirectory.append(fileData.cFileName);
			subDirectory.append("/");
			GetFileListInDirectory(subDirectory, indentLevel + 1);
		}
		// �����ǰ�ļ�����Ŀ¼
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