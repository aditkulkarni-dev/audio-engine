#include <vector>

class Queue{
    public:
    Queue()=default;

    void setSize(int size);
    void pushSample(float sample);
    void advance(int numSamples);
    int getAvailableSamples() const;
    void reset();
    void peekSamples(std::vector<float>& dest, int numSamples) const;

    private:
    std::vector<float> m_queue;
    int readIdx;
    int writeIdx;
    int m_availableSamples;


};