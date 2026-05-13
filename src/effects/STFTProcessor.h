#pragma once
#include "Effect.h"
#include "../dsp/CircularBuffer.h"
#include "../dsp/Queue.h"
#include <vector>
#include "kiss_fft.h"

class STFTProcessor : public Effect {
    public:
        STFTProcessor(int fftSize, int hopSize);
        virtual ~STFTProcessor();

        void process(AudioBuffer& buffer) override;
        std::unique_ptr<Effect> clone() const override;

    protected:
        virtual void processSpectrum(std::vector<kiss_fft_cpx>& bins) = 0;


    private:

        int m_fftSize;
        int m_hopSize;

        Queue m_inputQueue; // input audio samples
        Queue m_outputQueue;  // processed audio samples to be output
        std::vector<float> m_window; // hanns window or gaussian

        std::vector<float> m_timeFrame; // temporary buffer for t-domain samples
        std::vector<kiss_fft_cpx> m_freqBins; // temporary buffer for f-domain bins

        kiss_fft_cfg m_forwardConfig;
        kiss_fft_cfg m_inverseConfig;
        std::vector<kiss_fft_cpx> timeDomainCpx;


        
};