#include "MuteConverter.h"
#include "../Exceptions.h"

MuteConverter::MuteConverter(const std::vector<std::string>& params, uint32_t sampleRate) {
    if (params.size() < 2) {
        throw ConverterException("mute: требует 2 параметра (начало_сек конец_сек)");
    }

    try {
        double startSec = std::stod(params[0]);
        double endSec = std::stod(params[1]);

        if (startSec < 0.0) {
            throw ConverterException("mute: начало не может быть отрицательным");
        }
        if (endSec < 0.0) {
            throw ConverterException("mute: конец не может быть отрицательным");
        }
        if (startSec >= endSec) {
            throw ConverterException("mute: начало должно быть меньше конца");
        }

        startIndex = static_cast<size_t>(startSec * sampleRate);
        endIndex = static_cast<size_t>(endSec * sampleRate);
    } catch (const std::invalid_argument&) {
        throw ConverterException("mute: параметры должны быть числами");
    } catch (const std::out_of_range&) {
        throw ConverterException("mute: параметры вне допустимого диапазона");
    }
}

int16_t MuteConverter::process(int16_t sample, size_t sampleIndex) {
    if (sampleIndex >= startIndex && sampleIndex < endIndex) {
        return 0;
    }
    return sample;
}

std::string MuteConverter::getName() const {
    return "mute";
}

std::string MuteConverter::getDescription() const {
    return "Заглушить аудио в указанном интервале времени.";
}

std::string MuteConverter::getSyntax() const {
    return "mute <начало_сек> <конец_сек>";
}

std::string MuteConverter::getExample() const {
    return "mute 0 5\n"
           "          (тишина в первые 5 секунд)";
}

// help доделать, валидацию параметров из фабрики вынести в конвертеры

// 1 техническое требование доделать, минимизировать хранимые данные