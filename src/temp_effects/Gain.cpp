#include "Gain.h"
#include "../audio/AudioBuffer.h"

Gain::Gain(float gainAmount) : gain(gainAmount) {}

void Gain::process(AudioBlock& buffer){
    for (auto& sample : buffer.samples){
        sample *= gain;
    }
}

void Gain::setGain(float newGain){
    gain = newGain;
}

float Gain::getGain() const{
    return gain;
}