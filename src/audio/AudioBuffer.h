#pragma once
#include <cstddef>
#include <vector>
#include <iostream>

struct AudioClip {
    std::vector<float> samples;
    int sampleRate = 0;
    int numChannels = 0;
    size_t numFrames = 0;

    void displaySamples(int k){
        for(int i{0}; i<k; i++){
            std::cout << samples[i] << " ";
        }
        std::cout << "\n";
    }
};

struct AudioBlock {
    std::vector<float> samples;   // interleaved block
    int sampleRate = 0;
    int numChannels = 0;
    size_t numFrames = 0;
};

