#include <iostream>

using namespace std;

const char* szNewCommand = "New";
const char* szDelCommand = "Del";

int main()
{
	int** ppBuffer = nullptr;
	char szCommand[128];

	while (true)
	{
		cin >> szCommand;

		if (strcmp(szCommand, szNewCommand) == 0)
		{
			ppBuffer = new int*[10000];
			for (int i = 0; i < 10000; ++i)
			{
				ppBuffer[i] = new int[10000];
			}
		}
		else if (strcmp(szCommand, szDelCommand) == 0)
		{
			if (ppBuffer)
			{
				for (int i = 0; i < 10000; ++i)
				{
					delete[] ppBuffer[i];
				}
				delete[] ppBuffer;
				ppBuffer = nullptr;
			}
		}
	}

	return 0;
}

