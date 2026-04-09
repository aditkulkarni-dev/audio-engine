#include "EffectChain.h"
#include "../audio/AudioBuffer.h"
#include <memory>

void EffectChain::addEffect(std::unique_ptr<Effect> effectToAdd){

    // after pushback, effect[idx] points to the effect
    // after pushback, effectToAdd is set to nullptr

    effects.push_back(std::move(effectToAdd));

}

void EffectChain::process(AudioBlock& buffer){
    
    // Go through each effect and call its process function.
    // effect must modify the audio buffer in place
    
    for (auto& effect : effects){
        effect->process(buffer);
    }
}