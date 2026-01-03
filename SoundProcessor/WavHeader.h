#pragma once
#include <cstdint>
#include <string>

struct WavHeader {
    char riffHeader[4];
    uint32_t riffSize;
    char waveHeader[4];
    
    char fmtHeader[4];
    uint32_t fmtSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    
    char dataHeader[4];
    uint32_t dataSize;
    
    void initialize(uint32_t numSamples);
    void validate() const;
    uint32_t getDurationMs() const;
};