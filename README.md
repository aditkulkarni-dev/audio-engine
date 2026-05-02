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
## DSP Architecture:
The DSP layer is built around a polymorphic `Effect` interface.
Each effect implements a `process()` function that modifies `AudioBuffer` in place.
```
+-----------------------+
|      AudioEngine      |
|-----------------------|
| - input path          |
| - output path         |
| - EffectChain         |
| + addEffect(effect)   |
| + run()               |
+-----------|-----------+
            |
            | owns and applies
            v
+-----------------------+
|      EffectChain      |
|-----------------------|
| - vector<Effect>      |
| + addEffect(effect)   |
| + process(buffer)     |
| + clone()             |
+-----------|-----------+
            |
            | contains
            v
+-----------------------+
|   Effect Base Class   |
|-----------------------|
| + process(buffer) = 0 |
| + clone() = 0         |
+-----------|-----------+
            |
            | inherited by
            v
+-----------------------+        +-----------------------+
|         Gain          |        |        ffDelay         |
|-----------------------|        |-----------------------|
| - gainAmount          |        | - CircularBuffer      |
| + process(buffer)     |        | - delayTime           |
| + clone()             |        | - mix                 |
+-----------------------+        | + process(buffer)     |
                                 | + clone()             |
                                 +-----------|-----------+
                                             |
                                             | uses
                                             v
                                +------------------------+
                                |     CircularBuffer     |
                                |------------------------|
                                | + write(sample)        |
                                | + read(delayAmount)    |
                                | + reset()              |
                                +------------------------+
```
## Effect Processing Model:
Effects are processed through a linear `EffectChain`.
Each audio block goes through each effect inside the EffectChain and is modifed in place.
Having a linear effect chain keeps things simpler for now. In v2.0, it is planned to implement a DAG based effect system which allows for more complicated effects to be tested.

## Why Effects Are Cloned Per Channel:
Some effects are stateless such as `Gain`. 
Some effects are stateful such as a feedforward delay aka `ffDelay`.

Being stateful implies that the effect maintains its own memory, and may use samples from the past as an input for some operation.
Such stateful effects use a `CircularBuffer` to maintain their memory, so it is important that each channel gets its own CircularBuffer. 
The fundamental idea is that each channel must get its own effect/effectChain. That prevents mix ups due to both channels writing on the same CircularBuffer for example.

To solve this, every effect implements a `clone()` method. AudioEngine can then create independent effect chains for each channel while preserving the same effect order and parameters.

## Included Effects:

1) Gain:
   `Gain` scales every sample in the buffer by a fixed amount.
   Example:
   ```
   engine.addEffect(std::make_unique<Gain>(0.5f));
   ```
2) Feedforward Delay (ffDelay):
   `ffDelay` is a simple feedforward delay effect.
   Example:
   ```
   engine.addEffect(std::make_unique<ffDelay>(15000, 0.5f, 44100));
   ```
   This creates a delayed copy of the signal and mixes it with the original input.

## Project Structure:
```
audio-engine/
│
├── src/
│   ├── app/
│   │   └── main_engine.cpp
│   │
│   ├── audio/
│   │   ├── AudioBuffer.h
│   │   ├── AudioEngine.h
│   │   ├── AudioEngine.cpp
│   │   ├── AudioProcessor.h
│   │   ├── AudioProcessor.cpp
│   │   ├── BlockUtils.h
│   │   └── BlockUtils.cpp
│   │
│   ├── dsp/
│   │   ├── CircularBuffer.h
│   │   └── CircularBuffer.cpp
│   │
│   ├── effects/
│   │   ├── Effect.h
│   │   ├── EffectChain.h
│   │   ├── EffectChain.cpp
│   │   ├── Gain.h
│   │   ├── Gain.cpp
│   │   ├── ffDelay.h
│   │   └── ffDelay.cpp
│   │
│   ├── io/
│   │   ├── AudioFileIO.h
│   │   └── AudioFileIO.cpp
│   │
│   └── tests/
│       ├── BlockUtilsTest.cpp
│       ├── CircularBufferTest.cpp
│       ├── EffectChainTest.cpp
│       ├── GainTest.cpp
│       └── ffDelayTest.cpp
│
├── CMakeLists.txt
└── README.md
```
## Build Instructions (Linux Only):
Note: This build is designed for Linux. To make it work on Windows, one would have to set up vcpkg and install the dependencies that way and modify the CMakeLists.txt respectively.

Building this project is straightforward. You will need to install `libsndfile` for audio file I/O, and then compile the project using CMake.

1) Install Dependencies:
   ```
   sudo apt update
   sudo apt install libsndfile1-dev
   ```
2) Build with CMake:
   ```
   mkdir build && cd build
   cmake ..
   make
   ```
       
## Optional Unit Tests:
These will also be a part of v1.2 which will test the AudioEngine out directly. The individual components have already been tested in v1.0

## Current Limitations:
This project is still in active development.

Current limitations include:

1) The engine is currently offline, not a real-time audio callback system.
2) Effects currently process buffers in place.
3) The included effects are intentionally minimal.
4) The command-line interface is not yet developed.
5) The future graph/DAG-based routing system is not part of v1.1 yet.
6) Some tests may need to be updated after the v1.1 AudioBuffer refactor.

