# C++ Audio Processing Engine

A C++17 offline audio processing engine for rapidly prototyping digital signal processing (DSP) effects before moving them into a full plugin framework such as JUCE.

This project is designed as a lightweight development environment for audio developers. Instead of writing an effect inside a plugin, exporting a VST3, loading it into a DAW, testing it, and repeating that loop, this engine allows effects to be tested directly on audio files through a simple C++ API.

The current version, **v1.1**, introduces a higher-level `AudioEngine` interface, a unified `AudioBuffer` data structure, and a modular `EffectChain` system for adding and testing DSP effects.

---

## Current Version: v1.1

The original version of this project validated the low-level audio pipeline manually:
```
read WAV → deinterleave → split into blocks → process → merge → interleave → write WAV
```
Version 1.1 wraps that workflow inside the `AudioEngine` class so that the audio developers can focus on writing and testing effects instead of manually managing the full processing pipeline.

A typical usage now looks like this:
```
#include "audio/AudioEngine.h"
#include "effects/Gain.h"
#include "effects/ffDelay.h"

int main() {
    AudioEngine engine("./input.wav", "./output.wav");

    // Add effects here
    engine.addEffect(std::make_unique<Gain>(1.0f));
    engine.addEffect(std::make_unique<ffDelay>(15000, 1.0f, 44100));

    engine.run();

    return 0;
}
```

## High-Level Flow:
At a high level, the engine performs the following steps:
1) Input .wav file
2) AudioFileIO reads audio into an AudioBuffer
3) AudioEngine deinterleaves the audio into independent channel buffers
4) Each channel is split into processing blocks
5) Processed blocks are merged back into full channel buffers
6) Channels are interleaved back into a final AudioBuffer
7) AudioFileIO writes the output .wav file

The engine is currently an offline block-based processing system. It is inspired by real-time audio engine design because it processes audio in blocks, but its current purpose is offline DSP prototyping.

## Core Architecture:
1) `AudioEngine`:
   `AudioEngine` is the main entry point for the project, and owns the entire high-level flow.
   That keeps the user-facing code very simple as shown above.
2) `AudioBuffer`:
   `AudioBuffer` is the central audio data structure in v1.1. It represents audio data in memory and stores samples, sample rate, number of channels and frames.
   v1.0 used separate `AudioClip` and `AudioBlock` abstractions. In v1.1, these have been replaced by `AudioBuffer` to simplify the data model and generalize audio data structures.

   Now, an `AudioBuffer` can represent a full audio file, a single channel of audio and a smaller processing block.
3) `AudioFileIO`:
   `AudioFileIO` handles .wav file reading using `libsndfile`.
   Its responsibility is limited to file I/O.
4) `BlockUtils`:
   `BlockUtils` contain utility functions for transforming audio buffers during processing.
   Important utils include:
   1) deinterleaving audio into separate mono channel buffers
   2) interleaving separate mono channel buffers into a single interleaved multi-channel AudioBuffer
   3) splitting an AudioBuffer into into smaller buffers (batches) for block-based processing
   4) merging small buffers of audio (batches) into a single AudioBuffer
   These functions allow the engine to mimic the structure of real-time audio processing while still operating offline.

