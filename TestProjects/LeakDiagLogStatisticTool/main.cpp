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
		Log("Input file path : \"%s\".", argv[1]);

		if (!LeakDiagLogStatisticTool::GetInstance().LoadLog(argv[1])) {
			Log("Error: Failed to load log file.");
		}
		else if (!LeakDiagLogStatisticTool::GetInstance().ParseLog()) {
			Log("Error: Failed to parse log file.");
		}
		else if (argc == 2) {
			char cBuffer[64];
			time_t logTime = time(NULL);

			strftime(cBuffer, 64, " %Y-%m-%d %H-%M-%S", localtime(&logTime));

			string strFileName = "StatiscResult";
			strFileName.append(cBuffer);
			strFileName.append(".txt");

			Log("Output file path : \"%s\".", strFileName.c_str());

			LeakDiagLogStatisticTool::GetInstance().SaveResult(strFileName.c_str());
		}
		else if (argc > 2) {
			Log("Output file path : \"%s\".", argv[2]);

			LeakDiagLogStatisticTool::GetInstance().SaveResult(argv[2]);
		}
	}

	system("pause");

	return 0;
}