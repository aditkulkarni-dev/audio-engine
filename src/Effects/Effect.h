#pragma once
#include "../audio/AudioBuffer.h"

class Effect{
public:
    virtual ~Effect() = default;
    virtual void process(AudioBlock& Buffer) = 0;

    // Every effect must have a process
};