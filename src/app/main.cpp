#include <iostream>
#include <memory>
#include "../audio/AudioBuffer.h"
#include "../io/AudioFileIO.h"
#include "../audio/BlockUtils.h"
#include "../effects/Gain.h"
#include "../audio/AudioProcessor.h"

int main(){
    AudioFileIO fileIo;
    AudioClip clip = fileIo.readWav("./input.wav");

    std::cout << "Loaded file\n";
    std::cout << "Sample rate: " << clip.sampleRate << "\n";
    std::cout << "Initial 10 Samples : \n";
    clip.displaySamples(10);

    constexpr size_t blockSize = 512;
    auto blocks = splitIntoBlocks(clip, blockSize);
    
    AudioProcessor processor;
    processor.addEffect(std::make_unique<Gain>(0.4f));

    for (auto& block : blocks){
        processor.processBlock(block);
    }

    auto finalClip = mergeBlocks(blocks);
    std::cout << "Final 10 Samples : \n";
    finalClip.displaySamples(10);
    fileIo.writeWav("./output.wav", finalClip);

    std::cout << "Wrote output.wav\n";
}
