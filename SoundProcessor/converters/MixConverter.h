#pragma once

#include "Converter.h"
#include "../WavStreamReader.h"
#include <vector>
#include <string>
#include <cstdint>

class MixConverter : public Converter {
public:
    MixConverter(const std::vector<std::string>& params,
                 const std::vector<std::string>& inputFiles,
                 uint32_t sampleRate);

    int16_t process(int16_t sample, size_t sampleIndex) override;

    std::string getName() const override;
    std::string getDescription() const override;
    std::string getSyntax() const override;
    std::string getExample() const override;

private:
    WavStreamReader secondary;
    size_t offsetIndex = 0;
    std::vector<int16_t> secBuf;
    size_t secPos = 0;
    size_t secValid = 0;
    bool secEnded = false;

    int16_t nextSecondarySample();
};
