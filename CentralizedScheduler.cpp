#include <iostream>
#include <fstream>
#include <json/json.h>

#include "CentralizedScheduler.h"

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		cout << "Cofig file path not specified" << endl;
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

}

void RunWorkerInstance(const string& configFilePath)
{
	WorkerConfig workerConfig;

	ReadConfigFile(configFilePath, workerConfig);
}

bool ReadConfigFile(const string& configFilePath, MasterConfig& masterConfig)
{
	try
	{
		Json::Value masterConfigValue;
		ifstream configFile(configFilePath, ifstream::binary);

		configFile >> masterConfigValue;

		masterConfig.numberOfWorkers = masterConfigValue["WorkersCount"].asInt();
		
		for (Json::ValueIterator itr = masterConfigValue["TaskTimeFrame"].begin(); itr != masterConfigValue["TaskTimeFrame"].end(); ++itr)
		{
			masterConfig.taskTimeFrame.push_back(itr->asInt());
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
		Json::Value masterConfigValue;
		ifstream configFile(configFilePath, ifstream::binary);

		configFile >> masterConfigValue;

		workerConfig.numberOfSlots = masterConfigValue["NumberOfSlots"].asInt();

		return true;
	}
	catch (char* excp)
	{
		cout << "Failed to read config file at path " << configFilePath << " reason " << excp << endl;
		return false;
	}
}




