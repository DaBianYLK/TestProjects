#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "DB_LeakDiagLogStatisticTool.h"
#include <DB_Log.h>
#include <sstream>

using namespace DaBianYLK;
using namespace std;

LeakDiagLogStatisticTool::LeakDiagLogStatisticTool() {

}

LeakDiagLogStatisticTool::~LeakDiagLogStatisticTool() {

}

bool LeakDiagLogStatisticTool::LoadLog(const char* filePath) {
	return m_XmlDoc.LoadFile(filePath);
}

bool LeakDiagLogStatisticTool::ParseLog() {
	m_uTotalLeakSize = 0;

	TiXmlNode* pNode = m_XmlDoc.FirstChild("logdata");
	if (!pNode) {
		Log("Error: Failed to get XML node \"logdata\".");
		return false;
	}

	pNode = pNode->FirstChild("xs:schema");
	if (!pNode) {
		Log("Error: Failed to get XML node \"xs:schema\".");
		return false;
	}

	pNode = pNode->NextSibling("LEAKS");
	if (!pNode) {
		Log("Error: Failed to get XML node \"LEAKS\".");
		return false;
	}

	TiXmlNode* pStackNode = pNode->FirstChild("STACK");
	TiXmlNode* pFrameNode = NULL;
	TiXmlElement* pStackElement = NULL;
	TiXmlElement* pFrameElement = NULL;
	const char*	pAttributeValue = NULL;

	map<string, FunctionInfo*> mapFunctionInfo;	// 函数地址与函数信息之间的映射
	stringstream sstreamBuffer;

	FunctionInfo*	pFunctionInfo = NULL;
	LeakData*		pLeakData = NULL;

	while (pStackNode) {
		pStackElement = pStackNode->ToElement();

		pLeakData = new LeakData;

		// 获取Stack的内存泄露次数
		pAttributeValue = pStackElement->Attribute("numallocs");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"numallocs\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttributeValue;
		sstreamBuffer >> pLeakData->uAllocNum;

		// 获取Stack的平均内存泄露大小
		pAttributeValue = pStackElement->Attribute("size");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"size\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttributeValue;
		sstreamBuffer >> pLeakData->uSize;

		// 获取Stack的总内存泄露大小
		pAttributeValue = pStackElement->Attribute("totalsize");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"totalsize\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttributeValue;
		sstreamBuffer >> pLeakData->uTotalSize;

		// 统计程序总内存泄露大小
		m_uTotalLeakSize += pLeakData->uTotalSize;

		pNode = pStackNode->FirstChild("STACKSTATS");
		if (!pNode) {
			Log("Error: Failed to get XML node \"STACKSTATS\".");
			return false;
		}

		// 获取函数调用栈信息
		pFrameNode = pNode->NextSibling("FRAME");
		while (pFrameNode) {
			pFrameElement = pFrameNode->ToElement();

			// 获取函数地址
			string strAddress = "";
			pAttributeValue = pFrameElement->Attribute("addr");
			if (!pAttributeValue) {
				Log("Error: Failed to get XML node attribute \"addr\".");
			}
			sstreamBuffer.clear();
			sstreamBuffer << pAttributeValue;
			sstreamBuffer >> strAddress;

			// 根据函数地址判断函数信息是否已经存在
			map<string, FunctionInfo*>::iterator itFunctionInfoPair = mapFunctionInfo.find(strAddress);
			// 如果函数信息不存在
			if (itFunctionInfoPair == mapFunctionInfo.end()) {
				pFunctionInfo = new FunctionInfo;

				pAttributeValue = pFrameElement->Attribute("num");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"num\".");
				}

				// 获取函数所属dll名称
				pAttributeValue = pFrameElement->Attribute("dll");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"dll\".");
				}
				pFunctionInfo->strDllName = pAttributeValue;

				// 获取函数名称
				pAttributeValue = pFrameElement->Attribute("function");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"function\".");
				}
				pFunctionInfo->strFunctionName = pAttributeValue;
				//ReplaceUnderlineToColon(pFunctionInfo->strFunctionName);

				// 获取函数所在文件名
				pAttributeValue = pFrameElement->Attribute("filename");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"filename\".");
				}
				pFunctionInfo->strFileName = pAttributeValue;

				// 获取调用函数的所在行
				pAttributeValue = pFrameElement->Attribute("line");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"line\".");
				}
				pFunctionInfo->strLine = pAttributeValue;

				// 获取函数地址
				pFunctionInfo->strAddress = strAddress;

				// 统计函数的总内存泄露大小
				pFunctionInfo->uTotalLeakSize = pLeakData->uTotalSize;

				mapFunctionInfo.insert(make_pair(strAddress, pFunctionInfo));
			}
			// 如果函数信息存在
			else {
				pFunctionInfo = itFunctionInfoPair->second;

				// 统计函数的总内存泄露大小
				pFunctionInfo->uTotalLeakSize += pLeakData->uTotalSize;
			}

			// 往函数信息中插入泄露数据
			//pFunctionInfo->listRelevantLeakDataPtr.push_back(pLeakData);

			// 往泄露数据中插入函数信息
			pLeakData->listRelevantFunctionInfoPtr.push_back(pFunctionInfo);

			pFrameNode = pFrameNode->NextSibling("FRAME");
		}

		// 将泄露数据插入链表，按泄露总内存降序排列（越大越前）
		//InsertLeakDataPtr(pLeakData);
		delete pLeakData;

		pStackNode = pStackNode->NextSibling("STACK");
	}

	// 将函数按总内存泄露大小降序插入到链表中
	map<string, FunctionInfo*>::iterator itFunctionInfoPair = mapFunctionInfo.begin();
	while (itFunctionInfoPair != mapFunctionInfo.end()) {
		InsertFunctionInfoPtr(itFunctionInfoPair->second);

		++itFunctionInfoPair;
	}

	mapFunctionInfo.clear();

	return true;
}

/*
bool LeakDiagLogStatisticTool::ParseLog() {
	m_uTotalLeakSize = 0;

	TiXmlNode* pNode = m_XmlDoc.FirstChild("logdata");
	if (!pNode) {
		Log("Error: Failed to get XML node \"logdata\".");
		return false;
	}

	pNode = pNode->FirstChild("xs:schema");
	if (!pNode) {
		Log("Error: Failed to get XML node \"xs:schema\".");
		return false;
	}

	pNode = pNode->NextSibling("LEAKS");
	if (!pNode) {
		Log("Error: Failed to get XML node \"LEAKS\".");
		return false;
	}

	TiXmlNode* pStackNode = pNode->FirstChild("STACK");
	TiXmlNode* pFrameNode = NULL;
	TiXmlElement* pStackElement = NULL;
	TiXmlElement* pFrameElement = NULL;
	TiXmlAttribute* pAttribute	= NULL;

	map<string, FunctionInfo*> mapFunctionInfo;	// 函数地址与函数信息之间的映射
	stringstream sstreamBuffer;

	FunctionInfo*	pFunctionInfo	= NULL;
	LeakData*		pLeakData		= NULL;

	while (pStackNode) {
		pStackElement = pStackNode->ToElement();

		pLeakData = new LeakData;

		// 获取Stack的内存泄露次数
		pAttribute = pStackElement->FirstAttribute();
		if (!pAttribute) {
			Log("Error: Failed to get XML node attribute \"numallocs\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttribute->Value();
		sstreamBuffer >> pLeakData->uAllocNum;

		// 获取Stack的平均内存泄露大小
		pAttribute = pAttribute->Next();
		if (!pAttribute) {
			Log("Error: Failed to get XML node attribute \"size\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttribute->Value();
		sstreamBuffer >> pLeakData->uSize;
		
		// 获取Stack的总内存泄露大小
		pAttribute = pAttribute->Next();
		if (!pAttribute) {
			Log("Error: Failed to get XML node attribute \"totalsize\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttribute->Value();
		sstreamBuffer >> pLeakData->uTotalSize;

		// 统计程序总内存泄露大小
		m_uTotalLeakSize += pLeakData->uTotalSize;

		pNode = pStackNode->FirstChild("STACKSTATS");
		if (!pNode) {
			Log("Error: Failed to get XML node \"STACKSTATS\".");
			return false;
		}

		// 获取函数调用栈信息
		pFrameNode = pNode->NextSibling("FRAME");
		while (pFrameNode) {
			pFrameElement = pFrameNode->ToElement();

			// 获取函数地址
			string strAddress = "";
			pAttribute = pFrameElement->LastAttribute();
			if (!pAttribute) {
				Log("Error: Failed to get XML node attribute \"addr\".");
			}
			sstreamBuffer.clear();
			sstreamBuffer << pAttribute->Value();
			sstreamBuffer >> strAddress;

			// 根据函数地址判断函数信息是否已经存在
			map<string, FunctionInfo*>::iterator itFunctionInfoPair = mapFunctionInfo.find(strAddress);
			// 如果函数信息不存在
			if (itFunctionInfoPair == mapFunctionInfo.end()) {
				pFunctionInfo = new FunctionInfo;

				pAttribute = pFrameElement->FirstAttribute();
				if (!pAttribute) {
					Log("Error: Failed to get XML node attribute \"num\".");
				}

				// 获取函数所属dll名称
				pAttribute = pAttribute->Next();
				if (!pAttribute) {
					Log("Error: Failed to get XML node attribute \"dll\".");
				}
				pFunctionInfo->strDllName = pAttribute->Value();

				// 获取函数名称
				pAttribute = pAttribute->Next();
				if (!pAttribute) {
					Log("Error: Failed to get XML node attribute \"function\".");
				}
				pFunctionInfo->strFunctionName = pAttribute->Value();
				//ReplaceUnderlineToColon(pFunctionInfo->strFunctionName);

				pAttribute = pAttribute->Next();
				if (!pAttribute) {
					Log("Error: Failed to get XML node attribute \"offset\".");
				}

				// 获取函数所在文件名
				pAttribute = pAttribute->Next();
				if (!pAttribute) {
					Log("Error: Failed to get XML node attribute \"filename\".");
				}
				pFunctionInfo->strFileName = pAttribute->Value();

				// 获取调用函数的所在行
				pAttribute = pAttribute->Next();
				if (!pAttribute) {
					Log("Error: Failed to get XML node attribute \"line\".");
				}
				pFunctionInfo->strLine = pAttribute->Value();

				// 获取函数地址
				pFunctionInfo->strAddress = strAddress;

				// 统计函数的总内存泄露大小
				pFunctionInfo->uTotalLeakSize = pLeakData->uTotalSize;

				mapFunctionInfo.insert(make_pair(strAddress, pFunctionInfo));
			}
			// 如果函数信息存在
			else {
				pFunctionInfo = itFunctionInfoPair->second;

				// 统计函数的总内存泄露大小
				pFunctionInfo->uTotalLeakSize += pLeakData->uTotalSize;
			}

			// 往函数信息中插入泄露数据
			pFunctionInfo->listRelevantLeakDataPtr.push_back(pLeakData);

			// 往泄露数据中插入函数信息
			pLeakData->listRelevantFunctionInfoPtr.push_back(pFunctionInfo);

			pFrameNode = pFrameNode->NextSibling("FRAME");
		}

		// 将泄露数据插入链表，按泄露总内存降序排列（越大越前）
		InsertLeakDataPtr(pLeakData);

		pStackNode = pStackNode->NextSibling("STACK");
	}

	// 将函数按总内存泄露大小降序插入到链表中
	map<string, FunctionInfo*>::iterator itFunctionInfoPair = mapFunctionInfo.begin();
	while (itFunctionInfoPair != mapFunctionInfo.end()) {
		InsertFunctionInfoPtr(itFunctionInfoPair->second);

		++itFunctionInfoPair;
	}

	mapFunctionInfo.clear();

	return true;
}
*/

void LeakDiagLogStatisticTool::SaveResult(const char* filePath) {
	FILE* pFile = fopen(filePath, "wb");

	fprintf(pFile, "TotalLeakSize = %d bytes\n\n", m_uTotalLeakSize);

	FunctionInfo* pFunctionInfo = NULL;
	list<FunctionInfo*>::iterator itFunctionDataPtr = m_listFunctionInfoPtr.begin();
	unsigned uIndex = 0;
	while (itFunctionDataPtr != m_listFunctionInfoPtr.end()) {
		pFunctionInfo = *itFunctionDataPtr;

		fprintf(pFile, "Index = %d\nDll = \"%s\"\nFunction = \"%s\"\nFilename = \"%s\"\nLine = \"%s\"\nAddress = \"%s\"\nLeakSize = %d bytes\n\n",
			uIndex,
			pFunctionInfo->strDllName.c_str(),
			pFunctionInfo->strFunctionName.c_str(),
			pFunctionInfo->strFileName.c_str(),
			pFunctionInfo->strLine.c_str(),
			pFunctionInfo->strAddress.c_str(),
			pFunctionInfo->uTotalLeakSize);
		fflush(pFile);

		++itFunctionDataPtr;
		++uIndex;
	}

	fclose(pFile);
}

void LeakDiagLogStatisticTool::InsertLeakDataPtr(LeakData* pLeakData) {
	list<LeakData*>::iterator itLeakDataPtr = m_listLeakDataPtr.begin();
	while (itLeakDataPtr != m_listLeakDataPtr.end()) {
		if ((*pLeakData) < (*(*itLeakDataPtr))) {
			++itLeakDataPtr;
		}
		else {
			m_listLeakDataPtr.insert(itLeakDataPtr, pLeakData);
			return;
		}
	}

	// 插入到链表末尾
	m_listLeakDataPtr.insert(itLeakDataPtr, pLeakData);
}

void LeakDiagLogStatisticTool::InsertFunctionInfoPtr(FunctionInfo* pFunctionInfo) {
	list<FunctionInfo*>::iterator itFunctionInfo = m_listFunctionInfoPtr.begin();
	while (itFunctionInfo != m_listFunctionInfoPtr.end()) {
		if ((*pFunctionInfo) < (*(*itFunctionInfo))) {
			++itFunctionInfo;
		}
		else {
			m_listFunctionInfoPtr.insert(itFunctionInfo, pFunctionInfo);
			return;
		}
	}

	// 插入到链表末尾
	m_listFunctionInfoPtr.insert(itFunctionInfo, pFunctionInfo);
}

void LeakDiagLogStatisticTool::ReplaceUnderlineToColon(string& strFunctionName) {
	unsigned uSize = strFunctionName.size();

	for (unsigned i = 1; i < uSize; ++i) {
		if (strFunctionName[i] == '_' && strFunctionName[i - 1] == '_') {
			strFunctionName[i - 1] = ':';
			strFunctionName[i] = ':';
		}
	}
}