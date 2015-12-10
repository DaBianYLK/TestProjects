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

	map<string, FunctionInfo*> mapFunctionInfo;	// ������ַ�뺯����Ϣ֮���ӳ��
	stringstream sstreamBuffer;

	FunctionInfo*	pFunctionInfo = NULL;
	LeakData*		pLeakData = NULL;

	while (pStackNode) {
		pStackElement = pStackNode->ToElement();

		pLeakData = new LeakData;

		// ��ȡStack���ڴ�й¶����
		pAttributeValue = pStackElement->Attribute("numallocs");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"numallocs\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttributeValue;
		sstreamBuffer >> pLeakData->uAllocNum;

		// ��ȡStack��ƽ���ڴ�й¶��С
		pAttributeValue = pStackElement->Attribute("size");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"size\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttributeValue;
		sstreamBuffer >> pLeakData->uSize;

		// ��ȡStack�����ڴ�й¶��С
		pAttributeValue = pStackElement->Attribute("totalsize");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"totalsize\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttributeValue;
		sstreamBuffer >> pLeakData->uTotalSize;

		// ͳ�Ƴ������ڴ�й¶��С
		m_uTotalLeakSize += pLeakData->uTotalSize;

		pNode = pStackNode->FirstChild("STACKSTATS");
		if (!pNode) {
			Log("Error: Failed to get XML node \"STACKSTATS\".");
			return false;
		}

		// ��ȡ��������ջ��Ϣ
		pFrameNode = pNode->NextSibling("FRAME");
		while (pFrameNode) {
			pFrameElement = pFrameNode->ToElement();

			// ��ȡ������ַ
			string strAddress = "";
			pAttributeValue = pFrameElement->Attribute("addr");
			if (!pAttributeValue) {
				Log("Error: Failed to get XML node attribute \"addr\".");
			}
			sstreamBuffer.clear();
			sstreamBuffer << pAttributeValue;
			sstreamBuffer >> strAddress;

			// ���ݺ�����ַ�жϺ�����Ϣ�Ƿ��Ѿ�����
			map<string, FunctionInfo*>::iterator itFunctionInfoPair = mapFunctionInfo.find(strAddress);
			// ���������Ϣ������
			if (itFunctionInfoPair == mapFunctionInfo.end()) {
				pFunctionInfo = new FunctionInfo;

				pAttributeValue = pFrameElement->Attribute("num");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"num\".");
				}

				// ��ȡ��������dll����
				pAttributeValue = pFrameElement->Attribute("dll");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"dll\".");
				}
				pFunctionInfo->strDllName = pAttributeValue;

				// ��ȡ��������
				pAttributeValue = pFrameElement->Attribute("function");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"function\".");
				}
				pFunctionInfo->strFunctionName = pAttributeValue;
				//ReplaceUnderlineToColon(pFunctionInfo->strFunctionName);

				// ��ȡ���������ļ���
				pAttributeValue = pFrameElement->Attribute("filename");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"filename\".");
				}
				pFunctionInfo->strFileName = pAttributeValue;

				// ��ȡ���ú�����������
				pAttributeValue = pFrameElement->Attribute("line");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"line\".");
				}
				pFunctionInfo->strLine = pAttributeValue;

				// ��ȡ������ַ
				pFunctionInfo->strAddress = strAddress;

				// ͳ�ƺ��������ڴ�й¶��С
				pFunctionInfo->uTotalLeakSize = pLeakData->uTotalSize;

				mapFunctionInfo.insert(make_pair(strAddress, pFunctionInfo));
			}
			// ���������Ϣ����
			else {
				pFunctionInfo = itFunctionInfoPair->second;

				// ͳ�ƺ��������ڴ�й¶��С
				pFunctionInfo->uTotalLeakSize += pLeakData->uTotalSize;
			}

			// ��������Ϣ�в���й¶����
			//pFunctionInfo->listRelevantLeakDataPtr.push_back(pLeakData);

			// ��й¶�����в��뺯����Ϣ
			pLeakData->listRelevantFunctionInfoPtr.push_back(pFunctionInfo);

			pFrameNode = pFrameNode->NextSibling("FRAME");
		}

		// ��й¶���ݲ���������й¶���ڴ潵�����У�Խ��Խǰ��
		//InsertLeakDataPtr(pLeakData);
		delete pLeakData;

		pStackNode = pStackNode->NextSibling("STACK");
	}

	// �����������ڴ�й¶��С������뵽������
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

	map<string, FunctionInfo*> mapFunctionInfo;	// ������ַ�뺯����Ϣ֮���ӳ��
	stringstream sstreamBuffer;

	FunctionInfo*	pFunctionInfo	= NULL;
	LeakData*		pLeakData		= NULL;

	while (pStackNode) {
		pStackElement = pStackNode->ToElement();

		pLeakData = new LeakData;

		// ��ȡStack���ڴ�й¶����
		pAttribute = pStackElement->FirstAttribute();
		if (!pAttribute) {
			Log("Error: Failed to get XML node attribute \"numallocs\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttribute->Value();
		sstreamBuffer >> pLeakData->uAllocNum;

		// ��ȡStack��ƽ���ڴ�й¶��С
		pAttribute = pAttribute->Next();
		if (!pAttribute) {
			Log("Error: Failed to get XML node attribute \"size\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttribute->Value();
		sstreamBuffer >> pLeakData->uSize;
		
		// ��ȡStack�����ڴ�й¶��С
		pAttribute = pAttribute->Next();
		if (!pAttribute) {
			Log("Error: Failed to get XML node attribute \"totalsize\".");
		}
		sstreamBuffer.clear();
		sstreamBuffer << pAttribute->Value();
		sstreamBuffer >> pLeakData->uTotalSize;

		// ͳ�Ƴ������ڴ�й¶��С
		m_uTotalLeakSize += pLeakData->uTotalSize;

		pNode = pStackNode->FirstChild("STACKSTATS");
		if (!pNode) {
			Log("Error: Failed to get XML node \"STACKSTATS\".");
			return false;
		}

		// ��ȡ��������ջ��Ϣ
		pFrameNode = pNode->NextSibling("FRAME");
		while (pFrameNode) {
			pFrameElement = pFrameNode->ToElement();

			// ��ȡ������ַ
			string strAddress = "";
			pAttribute = pFrameElement->LastAttribute();
			if (!pAttribute) {
				Log("Error: Failed to get XML node attribute \"addr\".");
			}
			sstreamBuffer.clear();
			sstreamBuffer << pAttribute->Value();
			sstreamBuffer >> strAddress;

			// ���ݺ�����ַ�жϺ�����Ϣ�Ƿ��Ѿ�����
			map<string, FunctionInfo*>::iterator itFunctionInfoPair = mapFunctionInfo.find(strAddress);
			// ���������Ϣ������
			if (itFunctionInfoPair == mapFunctionInfo.end()) {
				pFunctionInfo = new FunctionInfo;

				pAttribute = pFrameElement->FirstAttribute();
				if (!pAttribute) {
					Log("Error: Failed to get XML node attribute \"num\".");
				}

				// ��ȡ��������dll����
				pAttribute = pAttribute->Next();
				if (!pAttribute) {
					Log("Error: Failed to get XML node attribute \"dll\".");
				}
				pFunctionInfo->strDllName = pAttribute->Value();

				// ��ȡ��������
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

				// ��ȡ���������ļ���
				pAttribute = pAttribute->Next();
				if (!pAttribute) {
					Log("Error: Failed to get XML node attribute \"filename\".");
				}
				pFunctionInfo->strFileName = pAttribute->Value();

				// ��ȡ���ú�����������
				pAttribute = pAttribute->Next();
				if (!pAttribute) {
					Log("Error: Failed to get XML node attribute \"line\".");
				}
				pFunctionInfo->strLine = pAttribute->Value();

				// ��ȡ������ַ
				pFunctionInfo->strAddress = strAddress;

				// ͳ�ƺ��������ڴ�й¶��С
				pFunctionInfo->uTotalLeakSize = pLeakData->uTotalSize;

				mapFunctionInfo.insert(make_pair(strAddress, pFunctionInfo));
			}
			// ���������Ϣ����
			else {
				pFunctionInfo = itFunctionInfoPair->second;

				// ͳ�ƺ��������ڴ�й¶��С
				pFunctionInfo->uTotalLeakSize += pLeakData->uTotalSize;
			}

			// ��������Ϣ�в���й¶����
			pFunctionInfo->listRelevantLeakDataPtr.push_back(pLeakData);

			// ��й¶�����в��뺯����Ϣ
			pLeakData->listRelevantFunctionInfoPtr.push_back(pFunctionInfo);

			pFrameNode = pFrameNode->NextSibling("FRAME");
		}

		// ��й¶���ݲ���������й¶���ڴ潵�����У�Խ��Խǰ��
		InsertLeakDataPtr(pLeakData);

		pStackNode = pStackNode->NextSibling("STACK");
	}

	// �����������ڴ�й¶��С������뵽������
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

	// ���뵽����ĩβ
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

	// ���뵽����ĩβ
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