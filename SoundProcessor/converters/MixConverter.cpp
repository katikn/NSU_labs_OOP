#include "MixConverter.h"
#include "../Exceptions.h"
#include <sstream>

MixConverter::MixConverter(const std::vector<std::string>& params,
                           const std::vector<std::string>& inputFiles,
                           uint32_t sampleRate)
    : secBuf(4096) {

    if (params.size() < 2) {
        throw ConverterException("mix: требует 2 параметра ($номер_потока смещение_сек)");
    }

    std::string streamRef = params[0];
    if (streamRef.empty() || streamRef[0] != '$') {
        throw ConverterException("mix: ссылка на поток должна начинаться с $ (напр. $2)");
    }

    try {
        int streamIndex = std::stoi(streamRef.substr(1)) - 1;
        if (streamIndex < 0 || streamIndex >= static_cast<int>(inputFiles.size())) {
            throw ConverterException("mix: неверный номер потока $" +
                std::to_string(streamIndex + 1) + " (доступны: $1 до $" +
                std::to_string(inputFiles.size()) + ")");
        }

        double offsetSec = std::stod(params[1]);
        if (offsetSec < 0.0) {
            throw ConverterException("mix: смещение не может быть отрицательным");
        }

        secondary = WavStreamReader(inputFiles[streamIndex]);
        if (secondary.sampleRate() != sampleRate) {
            throw ConverterException("mix: частоты дискретизации не совпадают");
        }

        offsetIndex = static_cast<size_t>(offsetSec * sampleRate);
    } catch (const std::invalid_argument& e) {
        throw ConverterException("mix: параметры должны быть числами");
    } catch (const std::out_of_range& e) {
        throw ConverterException("mix: параметры вне допустимого диапазона");
    }
}

int16_t MixConverter::nextSecondarySample() {
    if (secEnded) return 0;

    if (secPos >= secValid) {
        secValid = secondary.readSamples(secBuf.data(), secBuf.size());
        secPos = 0;
        if (secValid == 0) {
            secEnded = true;
            return 0;
        }
    }

    return secBuf[secPos++];
}

int16_t MixConverter::process(int16_t sample, size_t sampleIndex) {
    if (sampleIndex < offsetIndex) {
        return sample;
    }

    int16_t sec = nextSecondarySample();
    if (secEnded) {
        return sample;
    }

    int32_t mixed = (static_cast<int32_t>(sample) + static_cast<int32_t>(sec)) / 2;

    if (mixed > 32767) mixed = 32767;
    if (mixed < -32768) mixed = -32768;

    return static_cast<int16_t>(mixed);
}

std::string MixConverter::getName() const {
    return "mix";
}

std::string MixConverter::getDescription() const {
    return "Смешать два аудиопотока (усредение сэмплов).";
}

std::string MixConverter::getSyntax() const {
    return "mix $<номер_потока> <смещение_сек>";
}

std::string MixConverter::getExample() const {
    return "mix $2 10\n"
           "          (примешать 2-й входной файл начиная с 10-й секунды)";
}
