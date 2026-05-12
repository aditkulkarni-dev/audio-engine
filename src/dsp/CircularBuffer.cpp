#include "CircularBuffer.h"
#include <stdexcept>

void CircularBuffer::reset(){
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    writeIdx = 0;
}

int CircularBuffer::getWriteIdx() const{
    return writeIdx;
}

void CircularBuffer::setSize(int size){
    if (size <= 0) {
        throw std::invalid_argument("Buffer size must be greater than 0");
    }
    this->size = size;
    buffer.resize(size);
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    reset();
}

int CircularBuffer::getSize() const
{
    return size;
}

void CircularBuffer::write(float sample){
    buffer[writeIdx] = sample;
    writeIdx = (writeIdx + 1) % buffer.size();
}

float CircularBuffer::read(int delay) const{
    if (delay <= 0 || delay > buffer.size()) {
        throw std::invalid_argument("Invalid delay");
    }

    int idxToRead = writeIdx - delay;
    if (idxToRead < 0) {
        idxToRead += buffer.size();
    }
    return buffer[idxToRead];
}