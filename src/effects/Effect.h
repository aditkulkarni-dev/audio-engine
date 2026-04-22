#pragma once
#include "../audio/AudioBuffer.h"

class Effect{
public:
    virtual ~Effect() = default;
    // Every effect must have a process
    virtual void process(AudioBuffer& Buffer) = 0;

    
};