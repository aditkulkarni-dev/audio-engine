#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "../audio/AudioBuffer.h"
#include "../audio/AudioProcessor.h"
#include "../audio/BlockUtils.h"
#include "../io/AudioFileIO.h"

/**
 * AudioEngine
 *
 * High-level wrapper for rapid effect prototyping.
 *
 * Typical usage:
 *
 *   AudioEngine engine;
 *   engine.load("input.wav");
 *   engine.addEffect<MyEffect>(arg1, arg2);
 *   engine.run();
 *   engine.save("output.wav");
 *
 * Effects are instantiated once per channel so stateful effects
 * (e.g. delay lines) behave correctly in multi-channel audio.
 */
class AudioEngine {
public:
    // ------------------------------------------------------------------ //
    //  Construction                                                        //
    // ------------------------------------------------------------------ //

    /**
     * @param blockSize  Number of frames per processing block.
     *                   Smaller = lower latency simulation; larger = faster.
     *                   Must be a power of two (e.g. 256, 512, 1024).
     */
    explicit AudioEngine(size_t blockSize = 512);

    // ------------------------------------------------------------------ //
    //  I/O                                                                 //
    // ------------------------------------------------------------------ //

    /** Load a WAV file. Replaces any previously loaded audio. */
    void load(const std::string& inputPath);

    /** Write the processed result to a WAV file. Must call run() first. */
    void save(const std::string& outputPath) const;

    // ------------------------------------------------------------------ //
    //  Effect chain                                                        //
    // ------------------------------------------------------------------ //

    /**
     * Append an effect to the chain.
     *
     * The effect is constructed with the supplied arguments and cloned
     * once per channel, so each channel gets independent internal state.
     *
     * Example:
     *   engine.addEffect<ffDelay>(15000, 1.0f, 44100);
     *
     * @tparam EffectType   Any class derived from Effect.
     * @tparam Args         Constructor argument types (deduced).
     */
    template<typename EffectType, typename... Args>
    void addEffect(Args&&... args);

    /** Remove all effects from the chain. Processors are reset. */
    void clearEffects();

    // ------------------------------------------------------------------ //
    //  Processing                                                          //
    // ------------------------------------------------------------------ //

    /**
     * Process the loaded audio through the effect chain.
     * Must be called after load() and before save().
     */
    void run();

    // ------------------------------------------------------------------ //
    //  Diagnostics                                                         //
    // ------------------------------------------------------------------ //

    /** Print basic info about the loaded clip (channels, frames, SR). */
    void printInfo() const;

    /** Print the first n samples of each channel after processing. */
    void printSamples(int n = 10) const;

    // ------------------------------------------------------------------ //
    //  Accessors                                                           //
    // ------------------------------------------------------------------ //

    int         sampleRate()  const;
    int         numChannels() const;
    size_t      numFrames()   const;
    size_t      blockSize()   const;

private:
    // ------------------------------------------------------------------ //
    //  Internal helpers                                                    //
    // ------------------------------------------------------------------ //

    /** Rebuild one AudioProcessor per channel and re-apply the effect chain. */
    void rebuildProcessors();

    // ------------------------------------------------------------------ //
    //  State                                                               //
    // ------------------------------------------------------------------ //

    AudioFileIO   m_fileIo;
    AudioClip     m_inputClip;   // Raw loaded audio
    AudioClip     m_outputClip;  // Result after run()
    size_t        m_blockSize;
    bool          m_loaded  = false;
    bool          m_hasRun  = false;

    /**
     * Factory list — each entry is a callable that receives an
     * AudioProcessor and appends one freshly-constructed effect to it.
     * Storing factories (rather than effect instances) lets us create
     * independent copies for every channel.
     */
    using EffectFactory = std::function<void(AudioProcessor&)>;
    std::vector<EffectFactory> m_effectFactories;

    /** One processor per channel, rebuilt on every addEffect / clearEffects. */
    std::vector<AudioProcessor> m_processors;
};

// -------------------------------------------------------------------------- //
//  Template implementation (must live in the header)                         //
// -------------------------------------------------------------------------- //

template<typename EffectType, typename... Args>
void AudioEngine::addEffect(Args&&... args) {
    // Capture constructor arguments by value so the lambda is self-contained.
    // std::tuple lets us store a variadic pack without repeating the types.
    auto argTuple = std::make_tuple(std::forward<Args>(args)...);

    m_effectFactories.push_back(
        [argTuple](AudioProcessor& proc) mutable {
            std::apply(
                [&proc](auto&&... a) {
                    proc.addEffect(
                        std::make_unique<EffectType>(std::forward<decltype(a)>(a)...)
                    );
                },
                argTuple
            );
        }
    );

    // Keep processors in sync if audio is already loaded.
    if (m_loaded) {
        rebuildProcessors();
    }
}