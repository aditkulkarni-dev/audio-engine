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

std::vector<AudioClip> deinterleave(const AudioClip &clip){
    
    if (clip.samples.size() != clip.numFrames * clip.numChannels) {
    throw std::runtime_error("Invalid AudioClip: sample count does not match frames * channels");
    }

    int channels = clip.numChannels;
    int frames = clip.numFrames;
    std::vector<AudioClip> clips(channels);
    /*
        For 2 channel,
        clip = [C01 C11 C02 C12 C03 C13 ...C0N C1N]
        we need to obtain,
        clip1 = [C01 C02 ... C0N]
        clip2 = [C11 C12 ... C1N]

    */
    // Initialize each output clip
    for (int ch = 0; ch < channels; ch++) {
        clips[ch].numChannels = 1;
        clips[ch].numFrames = frames;
        clips[ch].sampleRate = clip.sampleRate;
        clips[ch].samples.resize(frames);
    }

    // Deinterleave
    for (int frame = 0; frame < frames; frame++) {
        for (int ch = 0; ch < channels; ch++) {
            clips[ch].samples[frame] = clip.samples[frame * channels + ch];
        }
    }

    return clips;
}

AudioClip interleave(const std::vector<AudioClip>& clips) {
    if (clips.empty()) {
        return AudioClip{};
    }

    int numChannels = static_cast<int>(clips.size());
    int numFrames = clips[0].numFrames;
    int sampleRate = clips[0].sampleRate;

    // Validate all clips
    for (const auto& clip : clips) {
        if (clip.numChannels != 1) {
            throw std::runtime_error("interleave: all input clips must be mono");
        }
        if (clip.numFrames != numFrames) {
            throw std::runtime_error("interleave: all clips must have the same number of frames");
        }
        if (clip.sampleRate != sampleRate) {
            throw std::runtime_error("interleave: all clips must have the same sample rate");
        }
        if (clip.samples.size() != static_cast<size_t>(numFrames)) {
            throw std::runtime_error("interleave: clip sample count does not match numFrames");
        }
    }

    AudioClip out;
    out.numChannels = numChannels;
    out.numFrames = numFrames;
    out.sampleRate = sampleRate;
    out.samples.resize(static_cast<size_t>(numFrames * numChannels));

    for (int frame = 0; frame < numFrames; ++frame) {
        for (int ch = 0; ch < numChannels; ++ch) {
            out.samples[frame * numChannels + ch] = clips[ch].samples[frame];
        }
    }

    return out;
}