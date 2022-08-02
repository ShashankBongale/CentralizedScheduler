#include <vector>
#include <unordered_map>
#include <queue>

using namespace std;


class Master
{
public:
	Master(const vector<int>& workerSlots, const vector<int>& taskDuration);
	~Master();

	void Run();

private:
	vector<int> m_workerRemainingSlots;
	queue<int> m_taskDuration;
	vector<bool> m_readyWorkers;
	bool m_isJobDone = false;


private:
	void SendTasksToWorker();
	void ListenToWorkerForTaskCompletion();
	void FreeSlot(const int& workerCount);
};

