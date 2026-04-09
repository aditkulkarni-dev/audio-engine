#pragma once
#include <string>
#include "../audio/AudioBuffer.h"

class AudioFileIO{

public:
    AudioClip readWav(const std::string& filePath);
    void writeWav(const std::string& filePath, const AudioClip& buffer);
    


};