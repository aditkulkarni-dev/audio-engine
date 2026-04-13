#include "effects/EffectChain.h"
#include "audio/AudioBuffer.h"
#include <gtest/gtest.h>
#include "../effects/Gain.h"

TEST(EffectChainTest, AudioRemainsUnchangedIfNoEffects){
    EffectChain effectChain;
    AudioBlock block;
    
    block.samples = {0.0f, 1.0f, 0.0f, -1.0f};
    effectChain.process(block);
    EXPECT_FLOAT_EQ(block.samples[0], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[1], 1.0f);
    EXPECT_FLOAT_EQ(block.samples[2], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[3], -1.0f);
    
}

TEST(EffectChainTest, ValidateGain){
    EffectChain effectChain;
    effectChain.addEffect(std::make_unique<Gain>(0.5));
    effectChain.addEffect(std::make_unique<Gain>(0.5));

    AudioBlock block;
    
    block.samples = {0.0f, 1.0f, 0.0f, -1.0f};
    effectChain.process(block);
    EXPECT_FLOAT_EQ(block.samples[0], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[1], 0.25f);
    EXPECT_FLOAT_EQ(block.samples[2], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[3], -0.25f);
}