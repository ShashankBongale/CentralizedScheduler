#include <iostream>
#include <unistd.h>
#include <zmq_addon.hpp>

#include "Worker.h"

static zmq::context_t ctx;

#define MICRO 1000000

Worker::Worker(const unsigned long& numberOfSlots, const int& workerSocket)
{
    m_numberOfSlots = numberOfSlots;
    m_workerSocket = workerSocket;
}

Worker::~Worker()
{

}

void Worker::Run()
{
    pthread_t getTaskThread;
    int iRetVal = pthread_create(&getTaskThread, nullptr, &GetTasks, this);
    if(!iRetVal)
    {
        cout << "Failed to create get task thread" << endl;
        return;
    }

    pthread_t performTaskThread;
    iRetVal = pthread_create(&performTaskThread, nullptr, &PermformTask, this);
    if(!iRetVal)
    {
        cout << "Failed to create perform task thread" << endl;
        return;
    }

    pthread_join(getTaskThread, nullptr);
    pthread_join(performTaskThread, nullptr);

}

void* Worker::GetTasks(void* self)
{
    Worker* selfObj = (Worker *)self;

    zmq::socket_t sock(ctx, zmq::socket_type::rep);

    string url = "tcp://127.0.0.1:" + selfObj->m_workerSocket;
    sock.connect(url);

    while(true)
    {
        zmq::message_t taskDurationMsg;
        sock.recv(taskDurationMsg);

        pthread_mutex_lock(&selfObj->m_taskQueueLock);
        string taskDuration = taskDurationMsg.to_string();
        selfObj->m_taskQueue.push(atoi(taskDuration.c_str()));
        pthread_mutex_unlock(&selfObj->m_taskQueueLock);

        string trialMessage;
        zmq::message_t clientMsg(trialMessage);
        sock.send(clientMsg, zmq::send_flags::none);

        if(taskDuration == "0")
        {
            cout << "Received terminate msg from master" << endl;
            break;
        }
    }

    return nullptr;

}

void* Worker::PermformTask(void* selfObject)
{
    Worker* selfObj = (Worker *)selfObject;

    zmq::socket_t taskCompleteAckSock(ctx, zmq::socket_type::req);
    taskCompleteAckSock.connect("tcp://127.0.0.1:6000");

    while(true)
    {
        pthread_mutex_lock(&selfObj->m_taskQueueLock);
        if(selfObj->m_taskQueue.empty())
        {
            pthread_mutex_unlock(&selfObj->m_taskQueueLock);
            usleep(2 * MICRO); //sleeping for 2 seconds
            continue;
        }

        int taskDuration = selfObj->m_taskQueue.front();
        selfObj->m_taskQueue.pop();

        pthread_mutex_unlock(&selfObj->m_taskQueueLock);

        if(taskDuration == 0)
            break;

        cout << "Performing task for " << taskDuration << "secs" << endl;

        usleep(taskDuration * MICRO);

        zmq::message_t taskCompleteMsg(to_string(selfObj->m_workerSocket));
        taskCompleteAckSock.send(taskCompleteMsg, zmq::send_flags::none);

    }

    return nullptr;
}
