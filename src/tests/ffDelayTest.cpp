#include <gtest/gtest.h>
#include "../effects/ffDelay.h"
#include "audio/AudioBuffer.h"

TEST(DelayTest, AddsDelayCopyAtCorrectSample) {
    ffDelay delay(3, 1.0f, 24);
    AudioBlock block;
    block.samples = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    delay.process(block);

    EXPECT_FLOAT_EQ(block.samples[0], 1.0f);  // dry
    EXPECT_FLOAT_EQ(block.samples[1], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[2], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[3], 1.0f);  // delayed copy
    EXPECT_FLOAT_EQ(block.samples[4], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[5], 0.0f);
}

TEST(DelayTest, DelayOfOneSampleWorksCorrectly) {
    ffDelay delay(1, 0.5f, 24);
    AudioBlock block;
    block.samples = {1.0f, 0.0f, 0.0f, 0.0f};

    delay.process(block);

    EXPECT_FLOAT_EQ(block.samples[0], 1.0f);
    EXPECT_FLOAT_EQ(block.samples[1], 0.5f);
    EXPECT_FLOAT_EQ(block.samples[2], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[3], 0.0f);
}

TEST(DelayTest, NegativeGainInvertsDelayedCopy) {
    ffDelay delay(2, -0.5f, 24);
    AudioBlock block;
    block.samples = {1.0f, 0.0f, 0.0f, 0.0f};

    delay.process(block);

    EXPECT_FLOAT_EQ(block.samples[0], 1.0f);
    EXPECT_FLOAT_EQ(block.samples[1], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[2], -0.5f);
    EXPECT_FLOAT_EQ(block.samples[3], 0.0f);
}


TEST(DelayTest, SilenceInProducesSilenceOut) {
    ffDelay delay(3, 0.8f, 24);
    AudioBlock block;
    block.samples = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    delay.process(block);

    EXPECT_FLOAT_EQ(block.samples[0], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[1], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[2], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[3], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[4], 0.0f);
}

TEST(DelayTest, CarriesDelayAcrossBlocks) {
    ffDelay delay(5, 1.0f, 24);

    AudioBlock block1;
    block1.samples = {1.0f, 0.0f, 0.0f};

    AudioBlock block2;
    block2.samples = {0.0f, 0.0f, 0.0f};

    delay.process(block1);
    delay.process(block2);

    EXPECT_FLOAT_EQ(block1.samples[0], 1.0f);
    EXPECT_FLOAT_EQ(block1.samples[1], 0.0f);
    EXPECT_FLOAT_EQ(block1.samples[2], 0.0f);

    EXPECT_FLOAT_EQ(block2.samples[0], 0.0f);
    EXPECT_FLOAT_EQ(block2.samples[1], 0.0f);
    EXPECT_FLOAT_EQ(block2.samples[2], 1.0f);  // delayed click appears here
}

TEST(DelayTest, DoesNotRepeatEchoes) {
    ffDelay delay(1, 0.5f, 24);
    AudioBlock block;
    block.samples = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    delay.process(block);

    EXPECT_FLOAT_EQ(block.samples[0], 1.0f);
    EXPECT_FLOAT_EQ(block.samples[1], 0.5f);
    EXPECT_FLOAT_EQ(block.samples[2], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[3], 0.0f);
    EXPECT_FLOAT_EQ(block.samples[4], 0.0f);
}