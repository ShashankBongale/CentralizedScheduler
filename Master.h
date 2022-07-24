#include <vector>
#include <unordered_map>
using namespace std;

class Master
{
public:
	Master(const vector<int>& slots, const vector<int>& taskDuration);
	~Master();

private:
	vector<int> m_workerRemainingSlots;
	vector<int> m_taskDuration;
	bool m_isJobDone = false;

private:
	void SendTasksToWorker();
	void ListenToWorkerForTaskCompletion();
	void FreeSlot(int workerCount);
};

Master::Master(const vector<int>& slots, const vector<int>& taskDuration)
{
	//Its assumed that tasks are loaded only one time in the start

	m_workerRemainingSlots.assign(slots.begin(), slots.end());
	m_taskDuration.assign(taskDuration.begin(), taskDuration.end());
}

Master::~Master()
{
}