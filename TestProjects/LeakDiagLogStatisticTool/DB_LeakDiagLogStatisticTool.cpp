#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "DB_LeakDiagLogStatisticTool.h"
#include <DB_Log.h>

using namespace DaBianYLK;
using namespace std;

#ifndef XML_SAX_PARSE
LeakDiagLogStatisticTool::LeakDiagLogStatisticTool() {

}

LeakDiagLogStatisticTool::~LeakDiagLogStatisticTool() {

}

bool LeakDiagLogStatisticTool::LoadLog(const char* filePath) {
	if (!m_XmlDoc.LoadFile(filePath)) {
		return false;
	}

	return ParseLog();
}

void LeakDiagLogStatisticTool::SaveResult(const char* filePath) {
	FILE* pFile = fopen(filePath, "wb");

	fprintf(pFile, "TotalLeakSize = %d bytes\n\n", m_uTotalLeakSize);

	FunctionInfo* m_pFunctionInfo = NULL;
	list<FunctionInfo*>::iterator itFunctionDataPtr = m_listFunctionInfoPtr.begin();
	unsigned uIndex = 0;
	while (itFunctionDataPtr != m_listFunctionInfoPtr.end()) {
		m_pFunctionInfo = *itFunctionDataPtr;

		fprintf(pFile, "Index = %d\nDll = \"%s\"\nFunction = \"%s\"\nFilename = \"%s\"\nLine = \"%s\"\nAddress = \"%s\"\nLeakSize = %d bytes\n\n",
			uIndex,
			m_pFunctionInfo->strDllName.c_str(),
			m_pFunctionInfo->strFunctionName.c_str(),
			m_pFunctionInfo->strFileName.c_str(),
			m_pFunctionInfo->strLine.c_str(),
			m_pFunctionInfo->strAddress.c_str(),
			m_pFunctionInfo->uTotalLeakSize);
		fflush(pFile);

		++itFunctionDataPtr;
		++uIndex;
	}

	fclose(pFile);
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

	map<string, FunctionInfo*> m_mapFunctionInfo;	// 函数地址与函数信息之间的映射
	stringstream m_sstreamBuffer;

	FunctionInfo*	m_pFunctionInfo = NULL;
	LeakData*		m_pLeakData = NULL;

	while (pStackNode) {
		pStackElement = pStackNode->ToElement();

		m_pLeakData = new LeakData;

		// 获取Stack的内存泄露次数
		pAttributeValue = pStackElement->Attribute("numallocs");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"numallocs\".");
		}
		m_sstreamBuffer.clear();
		m_sstreamBuffer << pAttributeValue;
		m_sstreamBuffer >> m_pLeakData->uAllocNum;

		// 获取Stack的平均内存泄露大小
		pAttributeValue = pStackElement->Attribute("size");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"size\".");
		}
		m_sstreamBuffer.clear();
		m_sstreamBuffer << pAttributeValue;
		m_sstreamBuffer >> m_pLeakData->uSize;

		// 获取Stack的总内存泄露大小
		pAttributeValue = pStackElement->Attribute("totalsize");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"totalsize\".");
		}
		m_sstreamBuffer.clear();
		m_sstreamBuffer << pAttributeValue;
		m_sstreamBuffer >> m_pLeakData->uTotalSize;

		// 统计程序总内存泄露大小
		m_uTotalLeakSize += m_pLeakData->uTotalSize;

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
			m_sstreamBuffer.clear();
			m_sstreamBuffer << pAttributeValue;
			m_sstreamBuffer >> strAddress;

			// 根据函数地址判断函数信息是否已经存在
			map<string, FunctionInfo*>::iterator itFunctionInfoPair = m_mapFunctionInfo.find(strAddress);
			// 如果函数信息不存在
			if (itFunctionInfoPair == m_mapFunctionInfo.end()) {
				m_pFunctionInfo = new FunctionInfo;

				pAttributeValue = pFrameElement->Attribute("num");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"num\".");
				}

				// 获取函数所属dll名称
				pAttributeValue = pFrameElement->Attribute("dll");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"dll\".");
				}
				m_pFunctionInfo->strDllName = pAttributeValue;

				// 获取函数名称
				pAttributeValue = pFrameElement->Attribute("function");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"function\".");
				}
				m_pFunctionInfo->strFunctionName = pAttributeValue;
				//ReplaceUnderlineToColon(m_pFunctionInfo->strFunctionName);

				// 获取函数所在文件名
				pAttributeValue = pFrameElement->Attribute("filename");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"filename\".");
				}
				m_pFunctionInfo->strFileName = pAttributeValue;

				// 获取调用函数的所在行
				pAttributeValue = pFrameElement->Attribute("line");
				if (!pAttributeValue) {
					Log("Error: Failed to get XML node attribute \"line\".");
				}
				m_pFunctionInfo->strLine = pAttributeValue;

				// 获取函数地址
				m_pFunctionInfo->strAddress = strAddress;

				// 统计函数的总内存泄露大小
				m_pFunctionInfo->uTotalLeakSize = m_pLeakData->uTotalSize;

				m_mapFunctionInfo.insert(make_pair(strAddress, m_pFunctionInfo));
			}
			// 如果函数信息存在
			else {
				m_pFunctionInfo = itFunctionInfoPair->second;

				// 统计函数的总内存泄露大小
				m_pFunctionInfo->uTotalLeakSize += m_pLeakData->uTotalSize;
			}

			// 往函数信息中插入泄露数据
			//m_pFunctionInfo->listRelevantLeakDataPtr.push_back(m_pLeakData);

			// 往泄露数据中插入函数信息
			m_pLeakData->listRelevantFunctionInfoPtr.push_back(m_pFunctionInfo);

			pFrameNode = pFrameNode->NextSibling("FRAME");
		}

		// 将泄露数据插入链表，按泄露总内存降序排列（越大越前）
		//InsertLeakDataPtr(m_pLeakData);
		delete m_pLeakData;

		pStackNode = pStackNode->NextSibling("STACK");
	}

	// 将函数按总内存泄露大小降序插入到链表中
	map<string, FunctionInfo*>::iterator itFunctionInfoPair = m_mapFunctionInfo.begin();
	while (itFunctionInfoPair != m_mapFunctionInfo.end()) {
		InsertFunctionInfoPtr(itFunctionInfoPair->second);

		++itFunctionInfoPair;
	}

	m_mapFunctionInfo.clear();

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

map<string, FunctionInfo*> m_mapFunctionInfo;	// 函数地址与函数信息之间的映射
stringstream m_sstreamBuffer;

FunctionInfo*	m_pFunctionInfo	= NULL;
LeakData*		m_pLeakData		= NULL;

while (pStackNode) {
pStackElement = pStackNode->ToElement();

m_pLeakData = new LeakData;

// 获取Stack的内存泄露次数
pAttribute = pStackElement->FirstAttribute();
if (!pAttribute) {
Log("Error: Failed to get XML node attribute \"numallocs\".");
}
m_sstreamBuffer.clear();
m_sstreamBuffer << pAttribute->Value();
m_sstreamBuffer >> m_pLeakData->uAllocNum;

// 获取Stack的平均内存泄露大小
pAttribute = pAttribute->Next();
if (!pAttribute) {
Log("Error: Failed to get XML node attribute \"size\".");
}
m_sstreamBuffer.clear();
m_sstreamBuffer << pAttribute->Value();
m_sstreamBuffer >> m_pLeakData->uSize;

// 获取Stack的总内存泄露大小
pAttribute = pAttribute->Next();
if (!pAttribute) {
Log("Error: Failed to get XML node attribute \"totalsize\".");
}
m_sstreamBuffer.clear();
m_sstreamBuffer << pAttribute->Value();
m_sstreamBuffer >> m_pLeakData->uTotalSize;

// 统计程序总内存泄露大小
m_uTotalLeakSize += m_pLeakData->uTotalSize;

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
m_sstreamBuffer.clear();
m_sstreamBuffer << pAttribute->Value();
m_sstreamBuffer >> strAddress;

// 根据函数地址判断函数信息是否已经存在
map<string, FunctionInfo*>::iterator itFunctionInfoPair = m_mapFunctionInfo.find(strAddress);
// 如果函数信息不存在
if (itFunctionInfoPair == m_mapFunctionInfo.end()) {
m_pFunctionInfo = new FunctionInfo;

pAttribute = pFrameElement->FirstAttribute();
if (!pAttribute) {
Log("Error: Failed to get XML node attribute \"num\".");
}

// 获取函数所属dll名称
pAttribute = pAttribute->Next();
if (!pAttribute) {
Log("Error: Failed to get XML node attribute \"dll\".");
}
m_pFunctionInfo->strDllName = pAttribute->Value();

// 获取函数名称
pAttribute = pAttribute->Next();
if (!pAttribute) {
Log("Error: Failed to get XML node attribute \"function\".");
}
m_pFunctionInfo->strFunctionName = pAttribute->Value();
//ReplaceUnderlineToColon(m_pFunctionInfo->strFunctionName);

pAttribute = pAttribute->Next();
if (!pAttribute) {
Log("Error: Failed to get XML node attribute \"offset\".");
}

// 获取函数所在文件名
pAttribute = pAttribute->Next();
if (!pAttribute) {
Log("Error: Failed to get XML node attribute \"filename\".");
}
m_pFunctionInfo->strFileName = pAttribute->Value();

// 获取调用函数的所在行
pAttribute = pAttribute->Next();
if (!pAttribute) {
Log("Error: Failed to get XML node attribute \"line\".");
}
m_pFunctionInfo->strLine = pAttribute->Value();

// 获取函数地址
m_pFunctionInfo->strAddress = strAddress;

// 统计函数的总内存泄露大小
m_pFunctionInfo->uTotalLeakSize = m_pLeakData->uTotalSize;

m_mapFunctionInfo.insert(make_pair(strAddress, m_pFunctionInfo));
}
// 如果函数信息存在
else {
m_pFunctionInfo = itFunctionInfoPair->second;

// 统计函数的总内存泄露大小
m_pFunctionInfo->uTotalLeakSize += m_pLeakData->uTotalSize;
}

// 往函数信息中插入泄露数据
m_pFunctionInfo->listRelevantLeakDataPtr.push_back(m_pLeakData);

// 往泄露数据中插入函数信息
m_pLeakData->listRelevantFunctionInfoPtr.push_back(m_pFunctionInfo);

pFrameNode = pFrameNode->NextSibling("FRAME");
}

// 将泄露数据插入链表，按泄露总内存降序排列（越大越前）
InsertLeakDataPtr(m_pLeakData);

pStackNode = pStackNode->NextSibling("STACK");
}

// 将函数按总内存泄露大小降序插入到链表中
map<string, FunctionInfo*>::iterator itFunctionInfoPair = m_mapFunctionInfo.begin();
while (itFunctionInfoPair != m_mapFunctionInfo.end()) {
InsertFunctionInfoPtr(itFunctionInfoPair->second);

++itFunctionInfoPair;
}

m_mapFunctionInfo.clear();

return true;
}
*/

void LeakDiagLogStatisticTool::InsertLeakDataPtr(LeakData* m_pLeakData) {
	list<LeakData*>::iterator itLeakDataPtr = m_listLeakDataPtr.begin();
	while (itLeakDataPtr != m_listLeakDataPtr.end()) {
		if ((*m_pLeakData) < (*(*itLeakDataPtr))) {
			++itLeakDataPtr;
		}
		else {
			m_listLeakDataPtr.insert(itLeakDataPtr, m_pLeakData);
			return;
		}
	}

	// 插入到链表末尾
	m_listLeakDataPtr.insert(itLeakDataPtr, m_pLeakData);
}

void LeakDiagLogStatisticTool::InsertFunctionInfoPtr(FunctionInfo* m_pFunctionInfo) {
	list<FunctionInfo*>::iterator itFunctionInfo = m_listFunctionInfoPtr.begin();
	while (itFunctionInfo != m_listFunctionInfoPtr.end()) {
		if ((*m_pFunctionInfo) < (*(*itFunctionInfo))) {
			++itFunctionInfo;
		}
		else {
			m_listFunctionInfoPtr.insert(itFunctionInfo, m_pFunctionInfo);
			return;
		}
	}

	// 插入到链表末尾
	m_listFunctionInfoPtr.insert(itFunctionInfo, m_pFunctionInfo);
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
#else
using namespace xercesc_3_1;

LeakDiagLogStatisticTool::LeakDiagLogStatisticTool() {
	XMLPlatformUtils::Initialize();
}

LeakDiagLogStatisticTool::~LeakDiagLogStatisticTool() {
	XMLPlatformUtils::Terminate();
}

bool LeakDiagLogStatisticTool::LoadLog(const char* filePath) {
	// 定义XML读取器
	try {
		m_pXmlReader = XMLReaderFactory::createXMLReader();
	}
	catch (const XMLException& exception) {
		Log("Error: %s", XMLString::transcode(exception.getMessage()));
		Log("Error: Failed to create xml reader.");

		return false;
	}
	
	m_pXmlReader->setFeature(XMLUni::fgSAX2CoreValidation, true);
	m_pXmlReader->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
	m_pXmlReader->setFeature(XMLUni::fgXercesDynamic, false);
	m_pXmlReader->setFeature(XMLUni::fgXercesSchema, true);
	m_pXmlReader->setFeature(XMLUni::fgXercesSchemaFullChecking, true);

	// 定义Handler
	m_pXmlReader->setContentHandler(this);
	m_pXmlReader->setErrorHandler(this);
	m_pXmlReader->setEntityResolver(this);

	// 执行解析
	try {
		m_pXmlReader->parse(filePath);
	}
	catch (const XMLException& exception) {
		Log("Error: %s", XMLString::transcode(exception.getMessage()));
		Log("Error: Failed to parse xml file \"%s\" - %s", filePath);

		return false;
	}
	catch (const SAXParseException& exception) {
		Log("Error: %s", XMLString::transcode(exception.getMessage()));
		Log("Error: Failed to parse xml file \"%s\" - %s", filePath);

		return false;
	}
	catch (...) {
		Log("Error: Unknown exception occur.");
		Log("Error: Failed to parse xml file \"%s\" - %s", filePath);

		return false;
	}

	return true;
}

void LeakDiagLogStatisticTool::SaveResult(const char* filePath) {
	FILE* pFile = fopen(filePath, "wb");

	fprintf(pFile, "TotalLeakSize = %d bytes\n\n", m_uTotalLeakSize);

	FunctionInfo* m_pFunctionInfo = NULL;
	list<FunctionInfo*>::iterator itFunctionDataPtr = m_listFunctionInfoPtr.begin();
	unsigned uIndex = 0;
	while (itFunctionDataPtr != m_listFunctionInfoPtr.end()) {
		m_pFunctionInfo = *itFunctionDataPtr;

		fprintf(pFile, "Index = %d\nDll = \"%s\"\nFunction = \"%s\"\nFilename = \"%s\"\nLine = \"%s\"\nAddress = \"%s\"\nLeakSize = %d bytes\n\n",
			uIndex,
			m_pFunctionInfo->strDllName.c_str(),
			m_pFunctionInfo->strFunctionName.c_str(),
			m_pFunctionInfo->strFileName.c_str(),
			m_pFunctionInfo->strLine.c_str(),
			m_pFunctionInfo->strAddress.c_str(),
			m_pFunctionInfo->uTotalLeakSize);
		fflush(pFile);

		++itFunctionDataPtr;
		++uIndex;
	}

	fclose(pFile);
}

void LeakDiagLogStatisticTool::SaveSummary(const char* filePath) {
	FILE* pFile = fopen(filePath, "wb");

	FunctionInfo* m_pFunctionInfo = NULL;
	list<FunctionInfo*>::iterator itFunctionDataPtr = m_listFunctionInfoPtr.begin();
	unsigned uIndex = 0;
	while (itFunctionDataPtr != m_listFunctionInfoPtr.end()) {
		m_pFunctionInfo = *itFunctionDataPtr;

		fprintf(pFile, "%d,", uIndex);

		map<string, unsigned>* pFunctionSizeMap = NULL;
		map<string, unsigned>::iterator itFunctionSize;
		unsigned uSize = 0;
		list<map<string, unsigned>*>::iterator itFunctionSizeMapPtr = m_listpFunctionSizeMap.begin();
		while (itFunctionSizeMapPtr != m_listpFunctionSizeMap.end()) {
			pFunctionSizeMap = (*itFunctionSizeMapPtr);
			itFunctionSize = pFunctionSizeMap->find(m_pFunctionInfo->strAddress);
			if (itFunctionSize == pFunctionSizeMap->end()) {
				uSize = 0;
			}
			else {
				uSize = itFunctionSize->second;
			}

			fprintf(pFile, "%d,", uSize);

			++itFunctionSizeMapPtr;
		}

		fprintf(pFile, "\n");
		fflush(pFile);

		++itFunctionDataPtr;
		++uIndex;
	}

	fclose(pFile);
}

void LeakDiagLogStatisticTool::startDocument() {
	m_listLeakDataPtr.clear();
	m_listFunctionInfoPtr.clear();
	m_uTotalLeakSize = 0; 

	m_mapFunctionInfo.clear();
	m_sstreamBuffer.clear();
	m_pFunctionInfo = NULL;
	m_pLeakData = NULL;
}

void LeakDiagLogStatisticTool::endDocument() {
	//delete m_pXmlReader;

	// 创建函数地址与函数内存泄露大小的hash map
	map<string, unsigned>* pFunctionSizeMap = new map<string, unsigned>();

	// 将函数按总内存泄露大小降序插入到链表中，并构建mapFunctionSize
	map<string, FunctionInfo*>::iterator itFunctionInfoPair = m_mapFunctionInfo.begin();
	while (itFunctionInfoPair != m_mapFunctionInfo.end()) {
		InsertFunctionInfoPtr(itFunctionInfoPair->second);

		pFunctionSizeMap->insert(make_pair(itFunctionInfoPair->second->strAddress, itFunctionInfoPair->second->uTotalLeakSize));

		++itFunctionInfoPair;
	}

	m_listpFunctionSizeMap.push_back(pFunctionSizeMap);

	m_mapFunctionInfo.clear();
}

void LeakDiagLogStatisticTool::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs) {
	const XMLCh* pAttributeValue = NULL;

	if (wcscmp(qname, L"STACK") == 0) {
		m_pLeakData = new LeakData;

		// 获取Stack的内存泄露次数
		pAttributeValue = attrs.getValue(L"numallocs");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"numallocs\".");
		}
		m_sstreamBuffer.clear();
		m_sstreamBuffer << XMLString::transcode(pAttributeValue);
		m_sstreamBuffer >> m_pLeakData->uAllocNum;

		// 获取Stack的平均内存泄露大小
		pAttributeValue = attrs.getValue(L"size");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"size\".");
		}
		m_sstreamBuffer.clear();
		m_sstreamBuffer << XMLString::transcode(pAttributeValue);
		m_sstreamBuffer >> m_pLeakData->uSize;

		// 获取Stack的总内存泄露大小
		pAttributeValue = attrs.getValue(L"totalsize");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"totalsize\".");
		}
		m_sstreamBuffer.clear();
		m_sstreamBuffer << XMLString::transcode(pAttributeValue);
		m_sstreamBuffer >> m_pLeakData->uTotalSize;

		// 统计程序总内存泄露大小
		m_uTotalLeakSize += m_pLeakData->uTotalSize;
	}
	else if (wcscmp(qname, L"FRAME") == 0) {
		// 获取函数地址
		string strAddress = "";
		pAttributeValue = attrs.getValue(L"addr");
		if (!pAttributeValue) {
			Log("Error: Failed to get XML node attribute \"addr\".");
		}
		m_sstreamBuffer.clear();
		m_sstreamBuffer << XMLString::transcode(pAttributeValue);
		m_sstreamBuffer >> strAddress;

		// 根据函数地址判断函数信息是否已经存在
		map<string, FunctionInfo*>::iterator itFunctionInfoPair = m_mapFunctionInfo.find(strAddress);
		// 如果函数信息不存在
		if (itFunctionInfoPair == m_mapFunctionInfo.end()) {
			m_pFunctionInfo = new FunctionInfo;

			// 获取函数所属dll名称
			pAttributeValue = attrs.getValue(L"dll");
			if (!pAttributeValue) {
				Log("Error: Failed to get XML node attribute \"dll\".");
			}
			m_pFunctionInfo->strDllName = XMLString::transcode(pAttributeValue);

			// 获取函数名称
			pAttributeValue = attrs.getValue(L"function");
			if (!pAttributeValue) {
				Log("Error: Failed to get XML node attribute \"function\".");
			}
			m_pFunctionInfo->strFunctionName = XMLString::transcode(pAttributeValue);
			//ReplaceUnderlineToColon(m_pFunctionInfo->strFunctionName);

			// 获取函数所在文件名
			pAttributeValue = attrs.getValue(L"filename");
			if (!pAttributeValue) {
				Log("Error: Failed to get XML node attribute \"filename\".");
			}
			m_pFunctionInfo->strFileName = XMLString::transcode(pAttributeValue);

			// 获取调用函数的所在行
			pAttributeValue = attrs.getValue(L"line");
			if (!pAttributeValue) {
				Log("Error: Failed to get XML node attribute \"line\".");
			}
			m_pFunctionInfo->strLine = XMLString::transcode(pAttributeValue);

			// 获取函数地址
			m_pFunctionInfo->strAddress = strAddress;

			// 统计函数的总内存泄露大小
			m_pFunctionInfo->uTotalLeakSize = m_pLeakData->uTotalSize;

			m_mapFunctionInfo.insert(make_pair(strAddress, m_pFunctionInfo));
		}
		// 如果函数信息存在
		else {
			m_pFunctionInfo = itFunctionInfoPair->second;

			// 统计函数的总内存泄露大小
			m_pFunctionInfo->uTotalLeakSize += m_pLeakData->uTotalSize;
		}

		// 往函数信息中插入泄露数据
		//m_pFunctionInfo->listRelevantLeakDataPtr.push_back(m_pLeakData);

		// 往泄露数据中插入函数信息
		m_pLeakData->listRelevantFunctionInfoPtr.push_back(m_pFunctionInfo);
	}
}

void LeakDiagLogStatisticTool::characters(const XMLCh* const chars, const XMLSize_t length) {

}

void LeakDiagLogStatisticTool::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname) {
	if (wcscmp(qname, L"STACK") == 0) {
		// 将泄露数据插入链表，按泄露总内存降序排列（越大越前）
		//InsertLeakDataPtr(m_pLeakData);
		delete m_pLeakData;
	}
}

void LeakDiagLogStatisticTool::fatalError(const SAXParseException& exception) {

}


void LeakDiagLogStatisticTool::InsertLeakDataPtr(LeakData* m_pLeakData) {
	list<LeakData*>::iterator itLeakDataPtr = m_listLeakDataPtr.begin();
	while (itLeakDataPtr != m_listLeakDataPtr.end()) {
		if ((*m_pLeakData) < (*(*itLeakDataPtr))) {
			++itLeakDataPtr;
		}
		else {
			m_listLeakDataPtr.insert(itLeakDataPtr, m_pLeakData);
			return;
		}
	}

	// 插入到链表末尾
	m_listLeakDataPtr.insert(itLeakDataPtr, m_pLeakData);
}

void LeakDiagLogStatisticTool::InsertFunctionInfoPtr(FunctionInfo* m_pFunctionInfo) {
	list<FunctionInfo*>::iterator itFunctionInfo = m_listFunctionInfoPtr.begin();
	while (itFunctionInfo != m_listFunctionInfoPtr.end()) {
		if ((*m_pFunctionInfo) < (*(*itFunctionInfo))) {
			++itFunctionInfo;
		}
		else {
			m_listFunctionInfoPtr.insert(itFunctionInfo, m_pFunctionInfo);
			return;
		}
	}

	// 插入到链表末尾
	m_listFunctionInfoPtr.insert(itFunctionInfo, m_pFunctionInfo);
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
#endif

