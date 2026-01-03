#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include "WavHeader.h"

class WavStreamWriter {
public:
    WavStreamWriter(const std::string& filename, uint32_t sampleRate);

    void writeSamples(const int16_t* src, size_t count);
    void finalize();

private:
    std::ofstream file;
    WavHeader header{};
    uint32_t writtenSamples = 0;

    void writeHeader(const WavHeader& h);
};
