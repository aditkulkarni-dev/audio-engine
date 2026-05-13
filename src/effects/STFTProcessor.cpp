#include "STFTProcessor.h"
#include <cmath>

// CONSTRUCTOR

STFTProcessor::STFTProcessor(int fftSize, int hopSize):
m_fftSize(fftSize), m_hopSize(hopSize)
{
    m_timeFrame.resize(m_fftSize, 0.0f);
    m_freqBins.resize(m_fftSize);
    m_window.resize(m_fftSize, 0.0f);

    // generate a Hanns window
    for(int i{0}; i < m_fftSize; ++i){
        m_window[i] = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (m_fftSize - 1.0f)));
    }

    // allocate kissfft configs
    m_forwardConfig = kiss_fft_alloc(m_fftSize, 0, nullptr, nullptr);
    m_inverseConfig = kiss_fft_alloc(m_fftSize, 1, nullptr, nullptr);

    // queue size should be at least (user buffer size + fftSize)
    m_inputQueue.setSize(8192);
    m_inputQueue.reset();
    m_outputQueue.setSize(8192);
    m_outputQueue.reset();
    timeDomainCpx.resize(fftSize);
}

// DESTRUCTOR
STFTProcessor::~STFTProcessor(){
    kiss_fft_free(m_forwardConfig);
    kiss_fft_free(m_inverseConfig);
}

//PROCESS
void STFTProcessor::process(AudioBuffer& buffer){
    // write input buffer samples onto the input Queue
    for(float sample : buffer.samples){
        m_inputQueue.pushSample(sample);
    }

    while(m_inputQueue.getAvailableSamples() >= m_fftSize){
        m_inputQueue.peekSamples(m_timeFrame, m_fftSize);

        
        for (int i = 0; i < m_fftSize; ++i) {
            timeDomainCpx[i].r = m_timeFrame[i] * m_window[i]; 
            timeDomainCpx[i].i = 0.0f;
        }

        kiss_fft(m_forwardConfig, timeDomainCpx.data(), m_freqBins.data());

        processSpectrum(m_freqBins);

        kiss_fft(m_inverseConfig, m_freqBins.data(), timeDomainCpx.data());

        for (int i = 0; i < m_fftSize; ++i) {
            m_timeFrame[i] = timeDomainCpx[i].r / (float)m_fftSize;
            
            // Standard STFT requires windowing on the way back out too
            m_timeFrame[i] *= m_window[i]; 
        }

        m_outputQueue.addSamples(m_timeFrame, m_fftSize);
        m_outputQueue.advanceWrite(m_hopSize);

        m_inputQueue.advance(m_hopSize);

        
    }
    m_outputQueue.popSamples(buffer.samples, buffer.samples.size());

}


