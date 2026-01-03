#include "WavStreamReader.h"
#include "Exceptions.h"
#include <cstring>
#include <algorithm>

WavStreamReader::WavStreamReader(const std::string& filename)
    : file(filename, std::ios::binary) {

    if (!file.is_open()) {
        throw FileException("Cannot open file for reading: " + filename);
    }

    file.read(reinterpret_cast<char*>(&header.riffHeader), 4);
    file.read(reinterpret_cast<char*>(&header.riffSize), 4);
    file.read(reinterpret_cast<char*>(&header.waveHeader), 4);

    bool foundFmt = false;
    bool foundData = false;

    char chunkId[4];
    uint32_t chunkSize = 0;

    while (file.read(chunkId, 4)) {
        file.read(reinterpret_cast<char*>(&chunkSize), 4);

        if (std::memcmp(chunkId, "fmt ", 4) == 0) {
            file.read(reinterpret_cast<char*>(&header.audioFormat), 2);
            file.read(reinterpret_cast<char*>(&header.numChannels), 2);
            file.read(reinterpret_cast<char*>(&header.sampleRate), 4);
            file.read(reinterpret_cast<char*>(&header.byteRate), 4);
            file.read(reinterpret_cast<char*>(&header.blockAlign), 2);
            file.read(reinterpret_cast<char*>(&header.bitsPerSample), 2);

            std::memcpy(header.fmtHeader, "fmt ", 4);
            header.fmtSize = 16;

            if (chunkSize > 16) {
                file.ignore(chunkSize - 16);
            }
            foundFmt = true;
        } else if (std::memcmp(chunkId, "data", 4) == 0) {
            std::memcpy(header.dataHeader, "data", 4);
            header.dataSize = chunkSize;

            dataStart = file.tellg();
            dataSizeBytes = chunkSize;
            bytesRead = 0;

            foundData = true;
            break;
        } else {
            file.ignore(chunkSize);
        }
    }

    if (!foundFmt || !foundData) {
        throw FileException("No valid fmt/data chunks found in: " + filename);
    }

    header.validate();

    file.clear();
    file.seekg(dataStart);
}

uint32_t WavStreamReader::sampleRate() const {
    return header.sampleRate;
}

uint32_t WavStreamReader::totalSamples() const {
    return dataSizeBytes / 2;
}

size_t WavStreamReader::readSamples(int16_t* dst, size_t maxSamples) {
    if (bytesRead >= dataSizeBytes) {
        return 0;
    }

    uint32_t remainingBytes = dataSizeBytes - bytesRead;
    uint32_t wantBytes = static_cast<uint32_t>(maxSamples * 2);
    uint32_t toReadBytes = std::min(remainingBytes, wantBytes);

    file.read(reinterpret_cast<char*>(dst), toReadBytes);
    if (!file) {
        throw FileException("Failed to read WAV data");
    }

    bytesRead += toReadBytes;
    return toReadBytes / 2;
}
