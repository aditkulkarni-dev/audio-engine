C++ Audio Processing Engine

A real-time-capable digital audio processing engine written in modern C++ (C++17). This project implements a modular architecture for reading, manipulating, and writing audio files using custom digital signal processing (DSP) algorithms.

---

## Level 1: The Bird's Eye View (High-Level Flow)

At its highest level of abstraction, this engine acts as an automated studio pipeline. It ingests raw audio, breaks it down into manageable components, routes it through an effects processor, and reconstructs the final track.

Here is the life cycle of an audio signal moving through the engine:

```
 ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
 │  input.wav   │ ──> │ AudioFileIO  │ ──> │ AudioClip    │ (Interleaved Stereo)
 └──────────────┘     └──────────────┘     └──────┬───────┘
                                                  │
                                            [Deinterleave]
                                                  │
                      ┌───────────────────────────┴───────────────────────────┐
                      ▼                                                       ▼
               Mono Clip (Left)                                        Mono Clip (Right)
                      │                                                       │
              [SplitIntoBlocks]                                       [SplitIntoBlocks]
                      │                                                       │
                      ▼                                                       ▼
 ┌────────────────────────────────────────┐   ┌────────────────────────────────────────┐
 │ AudioProcessor (Left)                  │   │ AudioProcessor (Right)                 │
 │  ├─► [Block 1] ─► Gain ─► ffDelay ─►   │   │  ├─► [Block 1] ─► Gain ─► ffDelay ─►   │
 │  ├─► [Block 2] ─► Gain ─► ffDelay ─►   │   │  ├─► [Block 2] ─► Gain ─► ffDelay ─►   │
 │  └─► [Block N] ─► Gain ─► ffDelay ─►   │   │  └─► [Block N] ─► Gain ─► ffDelay ─►   │
 └────────────────────┬───────────────────┘   └────────────────────┬───────────────────┘
                      │                                            │
                [MergeBlocks]                                [MergeBlocks]
                      │                                            │
                      ▼                                            ▼
            Processed Mono (Left)                        Processed Mono (Right)
                      │                                            │
                      └───────────────────────────┬────────────────┘
                                                  │
                                            [Interleave]
                                                  │
                                                  ▼
 ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
 │  output.wav  │ ◄── │ AudioFileIO  │ ◄── │ AudioClip    │ (Processed Stereo)
 └──────────────┘     └──────────────┘     └──────────────┘
