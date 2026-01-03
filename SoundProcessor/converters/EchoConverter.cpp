#include "EchoConverter.h"
#include "../Exceptions.h"

EchoConverter::EchoConverter(const std::vector<std::string>& params, uint32_t sampleRate) {
    if (params.size() < 2) {
        throw ConverterException("echo: требует 2 параметра (задержка_сек затухание)");
    }

    try {
        double delaySec = std::stod(params[0]);
        double decayFactor = std::stod(params[1]);

        if (delaySec <= 0.0) {
            throw ConverterException("echo: задержка должна быть > 0");
        }
        if (decayFactor < 0.0 || decayFactor > 1.0) {
            throw ConverterException("echo: затухание должно быть между 0.0 и 1.0");
        }

        delayIndex = static_cast<size_t>(delaySec * sampleRate);
        if (delayIndex == 0) delayIndex = 1;

        this->decayFactor = decayFactor;
        ring.assign(delayIndex, 0);
        pos = 0;
    } catch (const std::invalid_argument&) {
        throw ConverterException("echo: параметры должны быть числами");
    } catch (const std::out_of_range&) {
        throw ConverterException("echo: параметры вне допустимого диапазона");
    }
}

int16_t EchoConverter::process(int16_t sample, size_t) {
    int32_t delayed = ring[pos];
    int32_t result = static_cast<int32_t>(sample) + static_cast<int32_t>(delayed * decayFactor);

    if (result > 32767) result = 32767;
    if (result < -32768) result = -32768;

    int16_t out = static_cast<int16_t>(result);
    ring[pos] = out;
    pos = (pos + 1) % delayIndex;

    return out;
}

std::string EchoConverter::getName() const {
    return "echo";
}

std::string EchoConverter::getDescription() const {
    return "Добавить эффект эхо/реверберации с задержкой и затуханием.";
}

std::string EchoConverter::getSyntax() const {
    return "echo <задержка_сек> <затухание>";
}

std::string EchoConverter::getExample() const {
    return "echo 0.5 0.7\n"
           "          (эхо с задержкой 0.5с и громкостью 70%)";
}
