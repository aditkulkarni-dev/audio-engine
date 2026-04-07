#pragma once
#include <memory>
#include <vector>
#include "AudioBuffer.h"
#include "Effect.h"

class EffectChain{
public:
    void addEffect(std::unique_ptr<Effect> effectToAdd);
    void process(AudioBuffer& buffer);

private:
    std::vector<std::unique_ptr<Effect>> effects;

};