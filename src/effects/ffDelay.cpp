#include "ffDelay.h"
#include "audio/AudioBuffer.h"

void ffDelay::setBufferSize(int bufferSize){
    buffer.setSize(bufferSize);
}

void ffDelay::process(AudioBuffer& Buffer){
    
    
    for (auto& sample : Buffer.samples){
        float input = sample;
        
        
        float delayed = buffer.read(delay);
 
        
        sample = input + mix * delayed;
        buffer.write(input);
     
        
    }
}

