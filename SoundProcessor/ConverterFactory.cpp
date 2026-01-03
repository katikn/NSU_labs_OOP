#include "ConverterFactory.h"
#include "converters/MuteConverter.h"
#include "converters/MixConverter.h"
#include "converters/EchoConverter.h"
#include "Exceptions.h"
#include <sstream>

std::unique_ptr<Converter> ConverterFactory::create(
    const std::string& name,
    const std::vector<std::string>& params,
    const std::vector<std::string>& inputFiles,
    uint32_t sampleRate) {

    if (name == "mute") {
        return std::make_unique<MuteConverter>(params, sampleRate);
    }

    if (name == "mix") {
        return std::make_unique<MixConverter>(params, inputFiles, sampleRate);
    }

    if (name == "echo") {
        return std::make_unique<EchoConverter>(params, sampleRate);
    }

    throw ConverterException("Неизвестный конвертер: " + name);
}

std::string ConverterFactory::getHelpText() {
    std::stringstream ss;
    ss << "\nДоступные конвертеры:\n";

    MuteConverter muteTemp({"0.1", "0.2"}, 44100);
    MixConverter mixTemp({"$2", "0.0"}, {"input.wav", "output.wav"}, 44100);
    EchoConverter echoTemp({"0.1", "0.5"}, 44100);

    ss << "\n  " << muteTemp.getDetailedHelp() << "\n";
    ss << "\n  " << mixTemp.getDetailedHelp() << "\n";
    ss << "\n  " << echoTemp.getDetailedHelp() << "\n";

    return ss.str();
}

// вынести валидацию параматеров в конвертер