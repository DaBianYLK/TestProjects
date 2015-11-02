#include "DB_ProcessManager.h"

using namespace std;
using namespace DaBianYLK;


int main(int argc, char *argv[]) {
	ProcessManager processManager;
	
	processManager.GetPrivilege();
	processManager.UpdateProcessInfoList();

	if (argc <= 1) {
		processManager.OutputProcessInfoList("ProcessList.csv");
	}
	else {
		processManager.OutputProcessInfoList(argv[1]);
	}
	
	//system("pause");

	return 0;
}