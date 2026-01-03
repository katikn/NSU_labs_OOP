#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include "WavHeader.h"

class WavStreamReader {
public:
    WavStreamReader() = default;
    explicit WavStreamReader(const std::string& filename);

    uint32_t sampleRate() const;
    uint32_t totalSamples() const;

    size_t readSamples(int16_t* dst, size_t maxSamples);

private:
    std::ifstream file;
    WavHeader header{};

    std::streampos dataStart{};
    uint32_t dataSizeBytes = 0;
    uint32_t bytesRead = 0;
};
