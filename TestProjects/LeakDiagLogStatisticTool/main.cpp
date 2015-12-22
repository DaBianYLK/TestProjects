#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "DB_LeakDiagLogStatisticTool.h"
#include <DB_Log.h>
#include <time.h>
#include <string>

using namespace DaBianYLK;
using namespace std;

int main(int argc, char **argv) {
	if (argc < 2 || !argv) {
		Log("Error: No file input.");
	}
	else {
		int iFileNum = argc - 1;
		char* logPath = NULL;

		for (int i = 0; i < iFileNum; ++i) {
			logPath = argv[i + 1];

			Log("Input file %d path : \"%s\".", i, logPath);

			if (LeakDiagLogStatisticTool::GetInstance().LoadLog(logPath)) {
				string strResultFileName = logPath;
				unsigned uExtensionPos = strResultFileName.find_last_of('.');
				if (uExtensionPos < strResultFileName.size()) {
					strResultFileName.assign(strResultFileName, 0, uExtensionPos);
				}
				strResultFileName.append("_Result.result");

				Log("Result file %d path : \"%s\".", i, strResultFileName.c_str());

				LeakDiagLogStatisticTool::GetInstance().SaveResult(strResultFileName.c_str());
			}
			else {
				Log("Error: Failed to load log file %d - \"%s\".", i, logPath);
			}
		}

		string strSummaryFileName = logPath;
		unsigned uExtensionPos = strSummaryFileName.find_last_of('.');
		if (uExtensionPos < strSummaryFileName.size()) {
			strSummaryFileName.assign(strSummaryFileName, 0, uExtensionPos);
		}
		strSummaryFileName.append("_Summary.csv");

		Log("Summary file path : \"%s\".", strSummaryFileName.c_str());

		LeakDiagLogStatisticTool::GetInstance().SaveSummary(strSummaryFileName.c_str());
	}

	system("pause");

	return 0;
}

//
//char cBuffer[64];
//time_t logTime = time(NULL);
//
//strftime(cBuffer, 64, " %Y-%m-%d %H-%M-%S", localtime(&logTime));
//
//string strFileName = "StatiscResult";
//strFileName.append(cBuffer);
//strFileName.append(".txt");
//
//Log("Output file path : \"%s\".", strFileName.c_str());
//
//LeakDiagLogStatisticTool::GetInstance().SaveResult(strFileName.c_str());