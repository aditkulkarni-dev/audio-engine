#pragma once
#include <string>
#include "../audio/AudioBuffer.h"

class AudioFileIO{

public:
    AudioBuffer readWav(const std::string& filePath);
    void writeWav(const std::string& filePath, const AudioBuffer& buffer);
    


};