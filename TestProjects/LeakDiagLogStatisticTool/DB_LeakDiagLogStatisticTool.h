#include <tinyxml.h>
#include <string>
#include <list>
#include <set>
#include <map>

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
		bool ParseLog();
		void SaveResult(const char* filePath);

	private:
		void InsertLeakDataPtr(LeakData* pLeakData);
		void InsertFunctionInfoPtr(FunctionInfo* pFunctionInfo);
		void ReplaceUnderlineToColon(std::string& strFunctionName);		// 将LeakDiag中函数名的下划线还原为域符号
	
	private:
		TiXmlDocument				m_XmlDoc;
		std::list<LeakData*>		m_listLeakDataPtr;
		std::list<FunctionInfo*>	m_listFunctionInfoPtr;

		unsigned					m_uTotalLeakSize;
	};
}

