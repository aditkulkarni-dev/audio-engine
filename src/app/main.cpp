#include <iostream>
#include <memory>
#include <vector>

#include "../audio/AudioBuffer.h"
#include "../audio/AudioProcessor.h"
#include "../audio/BlockUtils.h"
#include "../effects/Gain.h"
#include "../effects/ffDelay.h"
#include "../io/AudioFileIO.h"

int main() {
    AudioFileIO fileIo;
    AudioBuffer clip = fileIo.readWav("./input.wav");

    const int channels = clip.numChannels;
    constexpr size_t blockSize = 512;

    std::cout << "Loaded file\n";
    std::cout << "Number of channels: " << clip.numChannels << "\n";
    std::cout << "Number of frames: " << clip.numFrames << "\n";
    std::cout << "Sample rate: " << clip.sampleRate << "\n";


    // Deinterleave into mono channel clips
    std::vector<AudioBuffer> clips = deinterleave(clip);
    std::cout << "Shape of clips: (" << clips.size() << ", "
              << clips[0].samples.size() << ")\n";

    // Split each channel clip into blocks
    std::vector<std::vector<AudioBuffer>> blocks;
    blocks.reserve(channels);

    for (int channel = 0; channel < channels; ++channel) {
        blocks.push_back(splitIntoBlocks(clips[channel], blockSize));
    }

    // Create one processor per channel
    std::vector<AudioProcessor> processors(channels);
    for (int channel = 0; channel < channels; ++channel) {
        processors[channel].addEffect(std::make_unique<Gain>(1.0f));
        processors[channel].addEffect(std::make_unique<ffDelay>(15000, 1.0f, 44100));
    }

    // Process each block independently per channel
    for (int channel = 0; channel < channels; ++channel) {
        for (auto& block : blocks[channel]) {
            processors[channel].processBlock(block);
        }
    }

    // Merge blocks back into one clip per channel
    std::vector<AudioBuffer> finalClips;
    finalClips.reserve(channels);

    for (int channel = 0; channel < channels; ++channel) {
        finalClips.push_back(mergeBlocks(blocks[channel]));
    }

    std::cout << "Final 10 samples of channel 0:\n";

    // Interleave channels back into a single output clip
    AudioBuffer finalClip = interleave(finalClips);
    fileIo.writeWav("./output.wav", finalClip);

    std::cout << "Wrote output.wav\n";
    return 0;
}