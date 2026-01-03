#pragma once

#include "Converter.h"
#include <vector>
#include <string>
#include <cstdint>

class EchoConverter : public Converter {
public:
    EchoConverter(const std::vector<std::string>& params, uint32_t sampleRate);

    int16_t process(int16_t sample, size_t sampleIndex) override;

    std::string getName() const override;
    std::string getDescription() const override;
    std::string getSyntax() const override;
    std::string getExample() const override;

private:
    size_t delayIndex = 0;
    double decayFactor = 0.0;
    std::vector<int16_t> ring;
    size_t pos = 0;
};