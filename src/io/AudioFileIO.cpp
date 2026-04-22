#include "AudioFileIO.h"
#include "../audio/AudioBuffer.h"
#include <sndfile.h>
#include <stdexcept>

AudioBuffer AudioFileIO::readWav(const std::string& filePath) {
    SF_INFO info{};
    SNDFILE* file = sf_open(filePath.c_str(), SFM_READ, &info);

    if (!file) throw std::runtime_error("Failed to open file");

    AudioBuffer buffer;
    buffer.sampleRate = info.samplerate;
    buffer.numChannels = info.channels;
    buffer.numFrames = info.frames;


    buffer.samples.resize(buffer.numFrames * buffer.numChannels);

    sf_readf_float(file, buffer.samples.data(), buffer.numFrames);
    sf_close(file);

    return buffer;
}

void AudioFileIO::writeWav(const std::string& filePath, const AudioBuffer& buffer) {
    SF_INFO info{};
    info.samplerate = buffer.sampleRate;
    info.channels = buffer.numChannels;
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    SNDFILE* file = sf_open(filePath.c_str(), SFM_WRITE, &info);

    if (!file) throw std::runtime_error("Failed to open file");

    sf_writef_float(file, buffer.samples.data(), buffer.numFrames);
    sf_close(file);
}