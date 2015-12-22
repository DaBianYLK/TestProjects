// DOM模式解析xml需要占用很多内存，无法读取大型的xml文件，
// 因此在此使用SAX模式解析xml并加入编译开关
#define XML_SAX_PARSE

#ifndef XML_SAX_PARSE
#	include <tinyxml.h>
#else
#	include <xercesc/util/PlatformUtils.hpp>
#	include <xercesc/util/TransService.hpp>
#	include <xercesc/sax2/SAX2XMLReader.hpp>
#	include <xercesc/sax2/XMLReaderFactory.hpp>
#	include <xercesc/sax2/DefaultHandler.hpp>
#	include <xercesc/sax2/Attributes.hpp>
#endif

#include <string>
#include <list>
#include <set>
#include <map>
#include <sstream>

namespace DaBianYLK {
	struct LeakData;

	struct FunctionInfo {
		std::string strDllName;
		std::string strFunctionName;
		std::string strFileName;
		std::string strLine;
		std::string	strAddress;

		unsigned				uTotalLeakSize;
		std::list<LeakData*>	listRelevantLeakDataPtr;

		bool operator <(const FunctionInfo &info) const {
			return uTotalLeakSize < info.uTotalLeakSize;
		}
	};

	struct LeakData {
		unsigned	uAllocNum;
		unsigned	uSize;
		unsigned	uTotalSize;

		std::list<FunctionInfo*>	listRelevantFunctionInfoPtr;

		bool operator <(const LeakData &data) const {
			return uTotalSize < data.uTotalSize;
		}
	};


#ifndef XML_SAX_PARSE
	class LeakDiagLogStatisticTool {
	private:
		LeakDiagLogStatisticTool();

	public:
		~LeakDiagLogStatisticTool();

		static LeakDiagLogStatisticTool& GetInstance() {
			static LeakDiagLogStatisticTool instance;

			return instance;
		}

	public:
		bool LoadLog(const char* filePath);
		void SaveResult(const char* filePath);

	private:
		bool ParseLog();
		void InsertLeakDataPtr(LeakData* pLeakData);
		void InsertFunctionInfoPtr(FunctionInfo* pFunctionInfo);
		void ReplaceUnderlineToColon(std::string& strFunctionName);		// 将LeakDiag中函数名的下划线还原为域符号

	private:
		TiXmlDocument				m_XmlDoc;

		std::list<LeakData*>		m_listLeakDataPtr;
		std::list<FunctionInfo*>	m_listFunctionInfoPtr;

		unsigned					m_uTotalLeakSize;
	};
#else
	class LeakDiagLogStatisticTool : public xercesc_3_1::DefaultHandler {
	private:
		LeakDiagLogStatisticTool();

	public:
		~LeakDiagLogStatisticTool();

		static LeakDiagLogStatisticTool& GetInstance() {
			static LeakDiagLogStatisticTool instance;

			return instance;
		}

	public:
		bool LoadLog(const char* filePath);
		void SaveResult(const char* filePath);
		void SaveSummary(const char* filePath);

		// ================== SAX方式解析xml文件所需的接口 ==================
		void startDocument();
		void endDocument();
		void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc_3_1::Attributes& attrs);	// 获取开始标签的回调
		// 注意：一个标签的内容可能会分成多次读取，因此需要将每次读取的内容存到buffer中，在endElement中再处理
		void characters(const XMLCh* const chars, const XMLSize_t length);																			// 获取标签内容的回调
		void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);											// 获取结束标签的回调
		void fatalError(const xercesc_3_1::SAXParseException& exception);																			// 发生数据错误的回调

	private:
		// 解析过程中使用到的中间变量
		std::map<std::string, FunctionInfo*>	m_mapFunctionInfo;	// 函数地址与函数信息之间的映射
		std::stringstream				m_sstreamBuffer;
		FunctionInfo*				m_pFunctionInfo;
		LeakData*					m_pLeakData;

	private:
		void InsertLeakDataPtr(LeakData* pLeakData);
		void InsertFunctionInfoPtr(FunctionInfo* pFunctionInfo);
		void ReplaceUnderlineToColon(std::string& strFunctionName);		// 将LeakDiag中函数名的下划线还原为域符号

	private:
		xercesc_3_1::SAX2XMLReader* m_pXmlReader;

		std::list<LeakData*>		m_listLeakDataPtr;
		std::list<FunctionInfo*>	m_listFunctionInfoPtr;

		unsigned					m_uTotalLeakSize;

		std::list<std::map<std::string, unsigned>*> m_listpFunctionSizeMap;
	};
#endif

	
}

