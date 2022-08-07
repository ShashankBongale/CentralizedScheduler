#include <iostream>
#include <unistd.h>
#include <zmq_addon.hpp>

#include "Master.h"

static zmq::context_t ctx;

Master::Master(const vector<int>& workerSlots, const vector<int>& taskDuration)
{
    m_workerRemainingSlots.assign(workerSlots.begin(), workerSlots.end());

    for(vector<int>::const_iterator itr = taskDuration.begin(); itr != taskDuration.end(); ++itr)
    {
        m_taskDuration.push(*itr);
    }
}

Master::~Master()
{

}

void Master::Run()
{
    cout << "Starting master" << endl;
    
    sleep(10);
    zmq::socket_t sock(ctx, zmq::socket_type::req);
    sock.bind("tcp://127.0.0.1:5555");

    string trialMessage = "This is a message from server";
    zmq::message_t serverMsg(trialMessage);
    sock.send(serverMsg, zmq::send_flags::none);

    zmq::message_t clientReply;
    sock.recv(clientReply);

    cout << "Received " << clientReply.to_string() << " from client" << endl;

}
