#include "WavHeader.h"
#include "Exceptions.h"
#include <cstring>

void WavHeader::initialize(uint32_t numSamples) {
    std::memcpy(riffHeader, "RIFF", 4);
    riffSize = 36 + numSamples * 2;
    std::memcpy(waveHeader, "WAVE", 4);
    
    std::memcpy(fmtHeader, "fmt ", 4);
    fmtSize = 16;
    audioFormat = 1;
    numChannels = 1;
    sampleRate = 44100;
    bitsPerSample = 16;
    byteRate = sampleRate * numChannels * bitsPerSample / 8;
    blockAlign = numChannels * bitsPerSample / 8;
    
    std::memcpy(dataHeader, "data", 4);
    dataSize = numSamples * 2;
}

void WavHeader::validate() const {
    if (std::memcmp(riffHeader, "RIFF", 4) != 0) {
        throw UnsupportedFormatException("Invalid RIFF header");
    }
    if (std::memcmp(waveHeader, "WAVE", 4) != 0) {
        throw UnsupportedFormatException("Invalid WAVE header");
    }
    if (audioFormat != 1) {
        throw UnsupportedFormatException("Only PCM audio format is supported");
    }
    if (numChannels != 1) {
        throw UnsupportedFormatException("Only mono audio is supported");
    }
    if (sampleRate != 44100) {
        throw UnsupportedFormatException("Only 44100 Hz sample rate is supported");
    }
    if (bitsPerSample != 16) {
        throw UnsupportedFormatException("Only 16-bit audio is supported");
    }
}

uint32_t WavHeader::getDurationMs() const {
    if (sampleRate == 0) return 0;
    uint32_t numSamples = dataSize / 2;
    return (numSamples * 1000) / sampleRate;
}