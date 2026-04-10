#include <vector>
#include "AudioBuffer.h"

std::vector<AudioBlock> splitIntoBlocks(const AudioClip& clip, size_t blockSize);
AudioClip mergeBlocks(const std::vector<AudioBlock>& blocks);
std::vector<AudioClip> deinterleave(const AudioClip& clip);
AudioClip interleave(const std::vector<AudioClip>& clips);
