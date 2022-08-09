#include <iostream>
#include <fstream>
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
		cout << "Invalid mode, exiting" << endl;
		return 0;
	}

	return 0;
}

void RunMasterInstance(const string& configFilePath)
{
	MasterConfig masterConfig;

	ReadConfigFile(configFilePath, masterConfig);

	Master* masterInstance = new Master(masterConfig.workersSlots, masterConfig.workerSocket, masterConfig.tasksTimeFrame, masterConfig.taskCompleteAckSocket);

	masterInstance->Run();
}

void RunWorkerInstance(const string& configFilePath)
{
	WorkerConfig workerConfig;

	ReadConfigFile(configFilePath, workerConfig);

	Worker* workerInstance = new Worker(workerConfig.numberOfSlots, workerConfig.workerSocket, workerConfig.taskCompleteAckSocket);

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

		cout << "Workers socket: " << endl;
		for(Json::ValueIterator itr = masterConfigValue["WorkerSocket"].begin(); itr != masterConfigValue["WorkerSocket"].end(); ++itr)
		{
			masterConfig.workerSocket.push_back(itr->asInt());
			cout << itr->asInt() << " ";
		}
		cout << endl;

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

		masterConfig.taskCompleteAckSocket = masterConfigValue["TaskCompleteAckSocket"].asInt();
		cout << "Task complete acknowlegdement socket: " << masterConfig.taskCompleteAckSocket << endl;

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

		Json::Value workerConfigValue;
		ifstream configFile(configFilePath, ifstream::binary);

		configFile >> workerConfigValue;

		workerConfig.numberOfSlots = workerConfigValue["NumberOfSlots"].asInt();
		cout<< "Number of slots: " << workerConfig.numberOfSlots << endl;

		workerConfig.workerSocket = workerConfigValue["WorkerSlot"].asInt();
		cout << "Worker socket: " << workerConfig.workerSocket;

		workerConfig.taskCompleteAckSocket = workerConfigValue["TaskCompleteAckSocket"].asInt();
		cout << "Task complete acknowlegdement socket: " << workerConfig.taskCompleteAckSocket << endl;

		return true;
	}
	catch (char* excp)
	{
		cout << "Failed to read config file at path " << configFilePath << " reason " << excp << endl;
		return false;
	}
}
