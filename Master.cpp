#include <zmq.hpp>
#include "Master.h"

Master::Master(const vector<int>& workerSlots, const vector<int>& taskDuration)
{
    m_workerRemainingSlots.assign(workerSlots.begin(), workerSlots.end());

    for(vector<int>::const_iterator itr = taskDuration.begin(); itr != taskDuration.end(); ++itr)
    {
        m_taskDuration.push(*itr);
    }
}

~Master::Master()
{

}

void Master::Run()
{
    while(true)
    {
        if(m_taskDuration.empty())
            return;

    }
}
