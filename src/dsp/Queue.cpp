#include "Queue.h"

void Queue::setSize(int size)
{
    m_queue.resize(size, 0.0f);
}

void Queue::pushSample(float sample)
{
    if(m_availableSamples >= m_queue.size())
    {
        return;
    }
    m_queue[writeIdx] = sample;

    writeIdx =(writeIdx + 1) % m_queue.size();
    m_availableSamples++;
}

void Queue::advance(int numSamples)
{   
    numSamples =std::min(numSamples, m_availableSamples);
    readIdx = (readIdx + numSamples)% m_queue.size();
    m_availableSamples-=numSamples;
}

int Queue::getAvailableSamples() const
{   
    return m_availableSamples;
}

void Queue::reset()
{
    std::fill(m_queue.begin(), m_queue.end(), 0.0f);
    writeIdx = 0;
    readIdx = 0;
    m_availableSamples = 0;
}

void Queue::peekSamples(std::vector<float> &dest, int numSamples) const
{   
    dest.resize(numSamples);
    for(int i{0}; i<(numSamples); ++i){
        int idx = (readIdx + i) % m_queue.size();
        
        dest[i] = m_queue[idx]; 
    }

}

