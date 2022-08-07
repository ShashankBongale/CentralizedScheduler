#include <iostream>
#include <fstream>
// #include </usr/include/jsoncpp/json/json.h>
#include <json/json.h>
#include <zmq.hpp>

#include "CentralizedScheduler.h"
#include "Master.h"
#include "Worker.h"

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		cout << "Invalid command line arguments" << endl;
		return 0;
	}

	string mode = argv[1];
	string configFilePath = argv[2];

	if (mode == MASTERMODE)
		RunMasterInstance(configFilePath);
	else if (mode == WORKERMODE)
		RunWorkerInstance(configFilePath);
	else
	{
		cout << "Invalid mode. Exiting" << endl;
		return 0;
	}

	return 0;
}

void RunMasterInstance(const string& configFilePath)
{
	MasterConfig masterConfig;

	ReadConfigFile(configFilePath, masterConfig);

	Master* masterInstance = new Master(masterConfig.workersSlots, masterConfig.tasksTimeFrame);

	masterInstance->Run();
}

void RunWorkerInstance(const string& configFilePath)
{
	WorkerConfig workerConfig;

	ReadConfigFile(configFilePath, workerConfig);

	Worker* workerInstance = new Worker(0, 0);

	workerInstance->Run();

}

bool ReadConfigFile(const string& configFilePath, MasterConfig& masterConfig)
{
	try
	{
		cout << "Reading master config file" << endl;

		Json::Value masterConfigValue;
		ifstream configFile(configFilePath, ifstream::binary);

		configFile >> masterConfigValue;

		masterConfig.numberOfWorkers = masterConfigValue["WorkersCount"].asInt();

		cout << "Number of workers " << masterConfig.numberOfWorkers << endl;

		cout << "Workers slots: " << endl;

		for (Json::ValueIterator itr = masterConfigValue["WorkersSlots"].begin(); itr != masterConfigValue["WorkersSlots"].end(); ++itr)
		{
			masterConfig.workersSlots.push_back(itr->asInt());
			cout << itr->asInt() << " ";
		}
		cout << endl;

		for (Json::ValueIterator itr = masterConfigValue["TaskTimeFrame"].begin(); itr != masterConfigValue["TaskTimeFrame"].end(); ++itr)
		{
			masterConfig.tasksTimeFrame.push_back(itr->asInt());
		}

		return true;
	}
	catch(char* excp)
	{
		cout << "Failed to read config file at path " << configFilePath << " reason " << excp << endl;
		return false;
	}
}


bool ReadConfigFile(const string& configFilePath, WorkerConfig& workerConfig)
{
	try
	{
		cout << "Reading worker config file" << endl;

		Json::Value masterConfigValue;
		ifstream configFile(configFilePath, ifstream::binary);

		configFile >> masterConfigValue;

		workerConfig.numberOfSlots = masterConfigValue["NumberOfSlots"].asInt();
		workerConfig.workerId = masterConfigValue["WorkerId"].asInt();

		return true;
	}
	catch (char* excp)
	{
		cout << "Failed to read config file at path " << configFilePath << " reason " << excp << endl;
		return false;
	}
}
