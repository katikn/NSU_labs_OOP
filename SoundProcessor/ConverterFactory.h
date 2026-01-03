#pragma once

#include "converters/Converter.h"
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

class ConverterFactory {
public:
    static std::unique_ptr<Converter> create(
        const std::string &name,
        const std::vector<std::string> &params,
        const std::vector<std::string> &inputFiles,
        uint32_t sampleRate
    );

    static std::string getHelpText();
};
