#include <string>
#include <vector>
using namespace std;

#define MASTERMODE "master"
#define WORKERMODE "worker"

typedef struct MasterConfig
{
	int numberOfWorkers;
	int taskCompleteAckSocket;
	vector<int> tasksTimeFrame;
	vector<int> workersSlots;
	vector<int> workerSocket;

}MasterConfig;

typedef struct WorkerConfig
{
	int workerSocket;
	int numberOfSlots;
	int taskCompleteAckSocket;

}WorkerConfig;

bool ReadConfigFile(const string& configFilePath, MasterConfig& masterConfig);
bool ReadConfigFile(const string& configFilePath, WorkerConfig& workerConfig);

void RunMasterInstance(const string& configFilePath);
void RunWorkerInstance(const string& configFilePath);
