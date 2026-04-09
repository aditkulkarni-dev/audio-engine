#include <vector>
#include <algorithm>
#include "AudioBuffer.h"
#include "BlockUtils.h"

std::vector<AudioBlock> splitIntoBlocks(const AudioClip& clip, size_t blockSize) {
    std::vector<AudioBlock> blocks;

    size_t totalFrames = clip.numFrames;
    size_t channels = static_cast<size_t>(clip.numChannels);

    for (size_t startFrame = 0; startFrame < totalFrames; startFrame += blockSize) {
        size_t framesThisBlock = std::min(blockSize, totalFrames - startFrame);

        AudioBlock block;
        block.sampleRate = clip.sampleRate;
        block.numChannels = clip.numChannels;
        block.numFrames = framesThisBlock;
        block.samples.resize(framesThisBlock * channels);

        size_t startSample = startFrame * channels;
        size_t numSamples = framesThisBlock * channels;

        std::copy(
            clip.samples.begin() + startSample,
            clip.samples.begin() + startSample + numSamples,
            block.samples.begin()
        );

        blocks.push_back(std::move(block));
    }

    return blocks;
}

AudioClip mergeBlocks(const std::vector<AudioBlock>& blocks) {
    AudioClip clip;

    if (blocks.empty()) return clip;

    // Set metadata from first block
    clip.sampleRate = blocks[0].sampleRate;
    clip.numChannels = blocks[0].numChannels;

    size_t totalFrames = 0;
    for (const auto& block : blocks) {
        totalFrames += block.numFrames;
    }

    clip.numFrames = totalFrames;

    size_t totalSamples = totalFrames * clip.numChannels;
    clip.samples.reserve(totalSamples);

    // Merge samples
    for (const auto& block : blocks) {
        clip.samples.insert(
            clip.samples.end(),
            block.samples.begin(),
            block.samples.end()
        );
    }

    return clip;
}