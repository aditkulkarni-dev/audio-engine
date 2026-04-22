#include "AudioBuffer.h"
#include "../effects/EffectChain.h"
#include <memory>

class AudioProcessor {
public:
    AudioProcessor() = default;
    void processBlock(AudioBuffer& buffer);
    void addEffect(std::unique_ptr<Effect> effect);

private:
    EffectChain effectChain;

};