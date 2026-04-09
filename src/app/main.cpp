#include <iostream>
#include <memory>
#include "../audio/AudioBuffer.h"
#include "../io/AudioFileIO.h"
#include "../audio/BlockUtils.h"
#include "../effects/Effect.h"
#include "../effects/EffectChain.h"
#include "../effects/Gain.h"

int main(){
    AudioFileIO fileIo;
    AudioClip clip = fileIo.readWav("./input.wav");

    std::cout << "Loaded file\n";
    std::cout << "Sample rate: " << clip.sampleRate << "\n";
    std::cout << "Initial 10 Samples : \n";
    clip.displaySamples(10);

    constexpr size_t blockSize = 512;
    auto blocks = splitIntoBlocks(clip, blockSize);
    
    EffectChain chain;
    // add effects to the chain
    std::unique_ptr<Effect> gain = std::make_unique<Gain>(0.3f);

    chain.addEffect(std::move(gain));

    

    for (auto& block : blocks){
        chain.process(block);
    }

    auto finalClip = mergeBlocks(blocks);
    std::cout << "Final 10 Samples : \n";
    finalClip.displaySamples(10);
    fileIo.writeWav("../output.wav", finalClip);

    std::cout << "Wrote output.wav\n";
}
