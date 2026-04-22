#pragma once
#include <cstddef>
#include <vector>
#include <iostream>

class AudioBuffer{
    public:
    std::vector<float> samples;   // interleaved block
    int sampleRate = 0;
    int numChannels = 0;
    size_t numFrames = 0;
};

