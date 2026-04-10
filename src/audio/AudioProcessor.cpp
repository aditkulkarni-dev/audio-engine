#include "AudioProcessor.h"

void AudioProcessor::processBlock(AudioBlock& buffer){
    effectChain.process(buffer);
}

void AudioProcessor::addEffect(std::unique_ptr<Effect> effect){
    effectChain.addEffect(std::move(effect));
}