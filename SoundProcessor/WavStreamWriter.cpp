#include "WavStreamWriter.h"
#include "Exceptions.h"

WavStreamWriter::WavStreamWriter(const std::string& filename, uint32_t sampleRate)
    : file(filename, std::ios::binary) {

    if (!file.is_open()) {
        throw FileException("Cannot open file for writing: " + filename);
    }

    header.initialize(0);
    header.sampleRate = sampleRate;
    header.byteRate = header.sampleRate * header.numChannels * header.bitsPerSample / 8;
    header.blockAlign = header.numChannels * header.bitsPerSample / 8;

    writeHeader(header);
}

void WavStreamWriter::writeHeader(const WavHeader& h) {
    file.seekp(0);

    file.write(h.riffHeader, 4);
    file.write(reinterpret_cast<const char*>(&h.riffSize), 4);
    file.write(h.waveHeader, 4);

    file.write(h.fmtHeader, 4);
    file.write(reinterpret_cast<const char*>(&h.fmtSize), 4);
    file.write(reinterpret_cast<const char*>(&h.audioFormat), 2);
    file.write(reinterpret_cast<const char*>(&h.numChannels), 2);
    file.write(reinterpret_cast<const char*>(&h.sampleRate), 4);
    file.write(reinterpret_cast<const char*>(&h.byteRate), 4);
    file.write(reinterpret_cast<const char*>(&h.blockAlign), 2);
    file.write(reinterpret_cast<const char*>(&h.bitsPerSample), 2);

    file.write(h.dataHeader, 4);
    file.write(reinterpret_cast<const char*>(&h.dataSize), 4);

    if (!file) {
        throw FileException("Failed to write WAV header");
    }
}

void WavStreamWriter::writeSamples(const int16_t* src, size_t count) {
    file.write(reinterpret_cast<const char*>(src), static_cast<std::streamsize>(count * 2));
    if (!file) {
        throw FileException("Failed to write WAV data");
    }
    writtenSamples += static_cast<uint32_t>(count);
}

void WavStreamWriter::finalize() {
    header.initialize(writtenSamples);
    writeHeader(header);
    file.flush();
}
