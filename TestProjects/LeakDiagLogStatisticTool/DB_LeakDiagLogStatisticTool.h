// DOMģʽ����xml��Ҫռ�úܶ��ڴ棬�޷���ȡ���͵�xml�ļ���
// ����ڴ�ʹ��SAXģʽ����xml��������뿪��
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
		void ReplaceUnderlineToColon(std::string& strFunctionName);		// ��LeakDiag�к��������»��߻�ԭΪ�����

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

		// ================== SAX��ʽ����xml�ļ�����Ľӿ� ==================
		void startDocument();
		void endDocument();
		void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc_3_1::Attributes& attrs);	// ��ȡ��ʼ��ǩ�Ļص�
		// ע�⣺һ����ǩ�����ݿ��ܻ�ֳɶ�ζ�ȡ�������Ҫ��ÿ�ζ�ȡ�����ݴ浽buffer�У���endElement���ٴ���
		void characters(const XMLCh* const chars, const XMLSize_t length);																			// ��ȡ��ǩ���ݵĻص�
		void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);											// ��ȡ������ǩ�Ļص�
		void fatalError(const xercesc_3_1::SAXParseException& exception);																			// �������ݴ���Ļص�

	private:
		// ����������ʹ�õ����м����
		std::map<std::string, FunctionInfo*>	m_mapFunctionInfo;	// ������ַ�뺯����Ϣ֮���ӳ��
		std::stringstream				m_sstreamBuffer;
		FunctionInfo*				m_pFunctionInfo;
		LeakData*					m_pLeakData;

	private:
		void InsertLeakDataPtr(LeakData* pLeakData);
		void InsertFunctionInfoPtr(FunctionInfo* pFunctionInfo);
		void ReplaceUnderlineToColon(std::string& strFunctionName);		// ��LeakDiag�к��������»��߻�ԭΪ�����

	private:
		xercesc_3_1::SAX2XMLReader* m_pXmlReader;

		std::list<LeakData*>		m_listLeakDataPtr;
		std::list<FunctionInfo*>	m_listFunctionInfoPtr;

		unsigned					m_uTotalLeakSize;

		std::list<std::map<std::string, unsigned>*> m_listpFunctionSizeMap;
	};
#endif

	
}

