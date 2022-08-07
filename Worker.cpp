#include <iostream>
#include <zmq_addon.hpp>
#include "Worker.h"

static zmq::context_t ctx;

Worker::Worker(const unsigned long& numberOfSlots, const int& workerId)
{
    m_numberOfSlots = numberOfSlots;
    m_workerId = workerId;
}

Worker::~Worker()
{

}

void Worker::Run()
{
    // cout << "Starting worker" << endl;
    //
    // zmq::socket_t sock(ctx, zmq::socket_type::rep);
    // sock.connect("tcp://127.0.0.1:5555");
    //
    // zmq::message_t serverMsg;
    // sock.recv(serverMsg);
    //
    // cout << "Received " << serverMsg.to_string() << " from server" << endl;
    //
    // string trialMessage = "Message from client";
    // zmq::message_t clientMsg(trialMessage);
    //
    // sock.send(clientMsg, zmq::send_flags::none);

    pthread_t getTaskThread;
    int iRetVal = pthread_create(&getTaskThread, nullptr, &GetTasks, this);
    if(!iRetVal)
    {
        cout << "Failed to create get task thread" << endl;
        return;
    }

    pthread_t performTaskThread;
    iRetVal = pthread_create(&performTaskThread, nullptr, &PermformTask, this);
    if(!RetVal)
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

        pthread_mutex_lock(&m_taskQueueLock);
        string taskDuration = taskDurationMsg.to_string();
        selfObj->m_taskQueue.push(atoi(taskDuration));
        pthread_mutex_lock(&m_taskQueueLock);

        string trialMessage;
        zmq::message_t clientMsg(trialMessage);
        sock.send(clientMsg, zmq::send_flags::none);

        if(taskDuration == "0")
        {
            cout << "Received termintate msg from master" << endl;
            break;
        }
    }

}
