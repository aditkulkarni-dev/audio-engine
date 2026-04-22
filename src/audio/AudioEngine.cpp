#include "AudioEngine.h"

#include <iostream>
#include <stdexcept>

// -------------------------------------------------------------------------- //
//  Construction                                                               //
// -------------------------------------------------------------------------- //

AudioEngine::AudioEngine(size_t blockSize)
    : m_blockSize(blockSize) {}

// -------------------------------------------------------------------------- //
//  I/O                                                                        //
// -------------------------------------------------------------------------- //

void AudioEngine::load(const std::string& inputPath) {
    m_inputClip = m_fileIo.readWav(inputPath);
    m_loaded    = true;
    m_hasRun    = false;

    // Rebuild processors for the (possibly new) channel count.
    rebuildProcessors();

    std::cout << "[AudioEngine] Loaded \"" << inputPath << "\"\n";
    printInfo();
}

void AudioEngine::save(const std::string& outputPath) const {
    if (!m_hasRun) {
        throw std::runtime_error(
            "[AudioEngine] save() called before run(). "
            "Call run() first to process the audio.");
    }

    m_fileIo.writeWav(outputPath, m_outputClip);
    std::cout << "[AudioEngine] Saved \"" << outputPath << "\"\n";
}

// -------------------------------------------------------------------------- //
//  Effect chain                                                               //
// -------------------------------------------------------------------------- //

void AudioEngine::clearEffects() {
    m_effectFactories.clear();
    if (m_loaded) {
        rebuildProcessors();
    }
}

// -------------------------------------------------------------------------- //
//  Processing                                                                 //
// -------------------------------------------------------------------------- //

void AudioEngine::run() {
    if (!m_loaded) {
        throw std::runtime_error(
            "[AudioEngine] run() called before load(). "
            "Call load() first.");
    }

    const int channels = m_inputClip.numChannels;

    // --- 1. Deinterleave into per-channel clips ---
    std::vector<AudioClip> channelClips = deinterleave(m_inputClip);

    // --- 2. Split each channel into blocks ---
    std::vector<std::vector<AudioBlock>> blocks;
    blocks.reserve(channels);
    for (int ch = 0; ch < channels; ++ch) {
        blocks.push_back(splitIntoBlocks(channelClips[ch], m_blockSize));
    }

    // --- 3. Process every block through its channel's processor ---
    // Processors were already built with the current effect chain.
    // We reset them first so stateful effects start from a clean slate.
    rebuildProcessors();

    for (int ch = 0; ch < channels; ++ch) {
        for (auto& block : blocks[ch]) {
            m_processors[ch].processBlock(block);
        }
    }

    // --- 4. Merge blocks back into per-channel clips ---
    std::vector<AudioClip> finalClips;
    finalClips.reserve(channels);
    for (int ch = 0; ch < channels; ++ch) {
        finalClips.push_back(mergeBlocks(blocks[ch]));
    }

    // --- 5. Interleave channels into the output clip ---
    m_outputClip = interleave(finalClips);
    m_hasRun     = true;

    std::cout << "[AudioEngine] Processing complete.\n";
}

// -------------------------------------------------------------------------- //
//  Diagnostics                                                                //
// -------------------------------------------------------------------------- //

void AudioEngine::printInfo() const {
    if (!m_loaded) {
        std::cout << "[AudioEngine] No audio loaded.\n";
        return;
    }
    std::cout << "  Channels   : " << m_inputClip.numChannels << "\n"
              << "  Frames     : " << m_inputClip.numFrames   << "\n"
              << "  Sample rate: " << m_inputClip.sampleRate  << " Hz\n"
              << "  Block size : " << m_blockSize             << " frames\n";
}

void AudioEngine::printSamples(int n) const {
    const AudioClip& clip = m_hasRun ? m_outputClip : m_inputClip;

    if (!m_loaded) {
        std::cout << "[AudioEngine] No audio loaded.\n";
        return;
    }

    std::cout << (m_hasRun ? "[AudioEngine] First " : "[AudioEngine] Input first ")
              << n << " samples:\n";
    clip.displaySamples(n);
}

// -------------------------------------------------------------------------- //
//  Accessors                                                                  //
// -------------------------------------------------------------------------- //

int    AudioEngine::sampleRate()  const { return m_inputClip.sampleRate;  }
int    AudioEngine::numChannels() const { return m_inputClip.numChannels; }
size_t AudioEngine::numFrames()   const { return m_inputClip.numFrames;   }
size_t AudioEngine::blockSize()   const { return m_blockSize;             }

// -------------------------------------------------------------------------- //
//  Internal helpers                                                           //
// -------------------------------------------------------------------------- //

void AudioEngine::rebuildProcessors() {
    const int channels = m_loaded ? m_inputClip.numChannels : 0;
    m_processors.clear();
    m_processors.resize(channels);

    for (int ch = 0; ch < channels; ++ch) {
        for (auto& factory : m_effectFactories) {
            factory(m_processors[ch]);
        }
    }
}