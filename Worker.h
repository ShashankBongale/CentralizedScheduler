#include <queue>
#include <pthread.h>

using namespace std;

class Worker
{
public:
    Worker(const unsigned long& numberOfSlots, const int& workerSocket, const int& taskCompleteAckSock);
    ~Worker();

    void Run();
private:
    int m_numberOfSlots = 0;
    int m_workerSocket = 0;
    int m_taskCompleteAckSocket = 0;

    queue<int> m_taskQueue;
    pthread_mutex_t m_taskQueueLock;

private:
    static void* GetTasks(void* object);
    static void* PermformTask(void* object);
};
