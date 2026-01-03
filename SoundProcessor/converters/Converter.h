#pragma once

#include <cstdint>
#include <string>
#include <sstream>

class Converter {
public:
    virtual ~Converter() = default;

    virtual int16_t process(int16_t sample, size_t sampleIndex) = 0;

    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getSyntax() const = 0;
    virtual std::string getExample() const = 0;

    std::string getDetailedHelp() const {
        std::stringstream ss;
        ss << getSyntax() << "\n";
        ss << "  " << getDescription() << "\n";
        ss << "  Пример: " << getExample();
        return ss.str();
    }
};
