#include "STFTProcessor.h"
#include <cmath>

// CONSTRUCTOR

STFTProcessor::STFTProcessor(int fftSize, int hopSize):
m_fftSize(fftSize), m_hopSize(m_hopSize)
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
}

// DESTRUCTOR
STFTProcessor::~STFTProcessor(){
    kiss_fft_free(m_forwardConfig);
    kiss_fft_free(m_inverseConfig);
}


