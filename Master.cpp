#include <iostream>
#include <unistd.h>

#include "Master.h"

static zmq::context_t ctx;

Master::Master(const vector<int>& workerSlots, const vector<int>& workerSocket, const vector<int>& taskDuration,
const int& taskCompleteAckSocket)

{
    vector<int>::const_iterator itr1 = workerSocket.begin();
    vector<int>::const_iterator itr2 = workerSlots.begin();

    for(; itr1 != workerSocket.end() && itr2 != workerSlots.end(); ++itr1, ++itr2)
    {
        workerProperty workProp;
        workProp.remainingSlots = *itr2;

        workProp.sockPtr = new zmq::socket_t(ctx, zmq::socket_type::req);
        workProp.socketNumber = *itr1;
        string url = "tcp://127.0.0.1:" + to_string(workProp.socketNumber);
        workProp.sockPtr->bind(url);

        m_workerList.push_back(workProp);

    }

    for(vector<int>::const_iterator itr = taskDuration.begin(); itr != taskDuration.end(); ++itr)
    {
        m_taskDuration.push(*itr);
    }

    m_taskCompleteAckSocket = taskCompleteAckSocket;
}

Master::~Master()
{

}

void Master::Run()
{
    pthread_t taskPublishThread;
    int iRetVal = pthread_create(&taskPublishThread, nullptr, &Master::SendTasksToWorker, this);
    if(!iRetVal)
    {
        cout << "Failed to create task publish thread" << endl;
        return;
    }

    pthread_t taskCompleteAckThread;
    iRetVal = pthread_create(&taskCompleteAckThread, nullptr, &Master::ListenToWorkerForTaskCompletion, nullptr);
    if(!iRetVal)
    {
        cout << "Failed to create task complete acknowlegdement thread" << endl;
        return;
    }

    pthread_join(taskPublishThread, nullptr);
    pthread_join(taskCompleteAckThread, nullptr);

    cout << "Exiting master" << endl;
}

void* Master::SendTasksToWorker(void* object)
{
    Master* obj = (Master *)object;

    cout << "Starting to send tasks to worker" << endl;

    size_t numberOfWorkers = obj->m_workerList.size();
    size_t roundRobinCounter = 0;

    while(!obj->m_taskDuration.empty())
    {
        int selectedWorker = roundRobinCounter % numberOfWorkers;
        roundRobinCounter += 1;

        pthread_mutex_lock(&obj->m_workerSlotsLock);

        if(obj->m_workerList[selectedWorker].remainingSlots <= 0)
        {
            pthread_mutex_unlock(&obj->m_workerSlotsLock);
            continue;
        }

        int taskDurationTime = obj->m_taskDuration.front();
        obj->m_taskDuration.pop();
        zmq::message_t taskDurationMsg(to_string(taskDurationTime));

        cout << "Sending task with duration " << taskDurationTime << " for worker on socket " << obj->m_workerList[selectedWorker].socketNumber << endl;
        obj->m_workerList[selectedWorker].sockPtr->send(taskDurationMsg, zmq::send_flags::none);

        zmq::message_t clientReply;
        obj->m_workerList[selectedWorker].sockPtr->recv(clientReply); //this will be dummy msg just to ack

        obj->m_workerList[selectedWorker].remainingSlots -= 1;

        pthread_mutex_unlock(&obj->m_workerSlotsLock);
    }

    cout << "Sending term message to all workers" << endl;
    for(vector<workerProperty>::iterator itr = obj->m_workerList.begin(); itr != obj->m_workerList.end(); ++itr)
    {
        zmq::message_t terminateMsg("0");
        itr->sockPtr->send(terminateMsg, zmq::send_flags::none);

        zmq::message_t clientReply;
        itr->sockPtr->recv(clientReply);
    }

    return nullptr;
}

void* Master::ListenToWorkerForTaskCompletion(void* object)
{
    Master* obj = (Master *)object;

    zmq::socket_t sock(ctx, zmq::socket_type::rep);
    string url = "tcp://127.0.0.1:" + to_string(obj->m_taskCompleteAckSocket);
    sock.bind(url);

    while(true)
    {
        zmq::message_t taskCompleteMsg;
        sock.recv(taskCompleteMsg);

        string workerSocketNum = taskCompleteMsg.to_string();

        cout << "Received socket completion from worker on socket " << workerSocketNum << endl;
        
        for(vector<workerProperty>::iterator itr = obj->m_workerList.begin(); itr != obj->m_workerList.end(); ++itr)
        {
            if(itr->socketNumber == atoi(workerSocketNum.c_str()))
            {
                pthread_mutex_lock(&obj->m_workerSlotsLock);
                itr->remainingSlots += 1;
                pthread_mutex_lock(&obj->m_workerSlotsLock);
                break;
            }
        }
    }

    return nullptr;
}
