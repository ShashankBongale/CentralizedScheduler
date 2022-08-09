#include <vector>
#include <unordered_map>
#include <queue>
#include <pthread.h>

#include <zmq_addon.hpp>

using namespace std;

typedef struct workerProperty
{
	int remainingSlots;
	int socketNumber;
	zmq::socket_t* sockPtr;

}workerProperty;

class Master
{
public:
	Master(const vector<int>& workerSlots, const vector<int>& workerSocket, const vector<int>& taskDuration,
	const int& taskCompleteAckSocket);
	~Master();

	void Run();

private:

	bool m_isJobDone = false;
	int m_taskCompleteAckSocket = 0;
	pthread_mutex_t m_workerSlotsLock;

	vector<workerProperty> m_workerList;
	queue<int> m_taskDuration;
	vector<bool> m_readyWorkers;

private:
	static void* SendTasksToWorker(void* object);
	static void* ListenToWorkerForTaskCompletion(void* object);
	void FreeSlot(const int& workerCount);
};
