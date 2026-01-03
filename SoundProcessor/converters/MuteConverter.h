#pragma once

#include "Converter.h"
#include <vector>
#include <string>
#include <cstdint>

class MuteConverter : public Converter {
public:
    MuteConverter(const std::vector<std::string>& params, uint32_t sampleRate);

    int16_t process(int16_t sample, size_t sampleIndex) override;

    std::string getName() const override;
    std::string getDescription() const override;
    std::string getSyntax() const override;
    std::string getExample() const override;

private:
    size_t startIndex;
    size_t endIndex;
};
