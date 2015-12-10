#include "DB_Log.h"

#include <time.h>
#include <ostream>

using namespace std;

void DaBianYLK::Log(const char* format, ...) {
	char logMessage[MAX_LOG_LENGTH];
	time_t logTime = time(NULL);

	unsigned int length = strftime(logMessage, MAX_LOG_LENGTH, "%Y-%m-%d %X\t", localtime(&logTime));

	char* message = new char[MAX_LOG_LENGTH - length];
	va_list valueList;
	_crt_va_start(valueList, format);
	vsprintf(message, format, valueList);
	_crt_va_end(valueList);

	strcat(logMessage, message);
	strcat(logMessage, "\n");

	printf(logMessage);

	delete message;
}