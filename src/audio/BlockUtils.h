#include <vector>
#include "AudioBuffer.h"

std::vector<AudioBlock> splitIntoBlocks(const AudioClip& clip, size_t blockSize);
AudioClip mergeBlocks(const std::vector<AudioBlock>& blocks);
