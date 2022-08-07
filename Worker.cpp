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
    cout << "Starting worker" << endl;
    
    zmq::socket_t sock(ctx, zmq::socket_type::rep);
    sock.connect("tcp://127.0.0.1:5555");

    zmq::message_t serverMsg;
    sock.recv(serverMsg);

    cout << "Received " << serverMsg.to_string() << " from server" << endl;

    string trialMessage = "Message from cleint";
    zmq::message_t clientMsg(trialMessage);

    sock.send(clientMsg, zmq::send_flags::none);

}
