#pragma once
#include <memory>
#include <vector>
#include "../audio/AudioBuffer.h"
#include "Effect.h"

class EffectChain{
public:
    void addEffect(std::unique_ptr<Effect> effectToAdd);
    void process(AudioBlock& buffer);

private:
    std::vector<std::unique_ptr<Effect>> effects;

};