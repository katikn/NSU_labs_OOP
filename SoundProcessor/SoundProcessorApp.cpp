#include "SoundProcessorApp.h"
#include "WavStreamWriter.h"
#include "WavStreamReader.h"
#include "ConfigParser.h"
#include "ConverterFactory.h"
#include "Exceptions.h"
#include <iostream>

int SoundProcessorApp::run(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "-h") {
        printHelp();
        return 0;
    }

    if (argc < 5 || std::string(argv[1]) != "-c") {
        throw ArgumentException("Usage: sound_processor -c <config.txt> <output.wav> <input.wav> [input2.wav ...]");
    }

    std::string configFile = argv[2];
    std::string outputFile = argv[3];
    std::vector<std::string> inputFiles;

    for (int i = 4; i < argc; i++) {
        inputFiles.push_back(argv[i]);
    }

    processAudio(configFile, outputFile, inputFiles);
    return 0;
}

void SoundProcessorApp::printHelp() {
    std::cout << "SoundProcessor\n";
    std::cout << "Использование: sound_processor -c <config.txt> <output.wav> <input.wav> [input2.wav ...]\n\n";
    std::cout << "Параметры:\n";
    std::cout << "  -h              Показать это сообщение\n";
    std::cout << "  -c config.txt   Конфиг файл с параметрами\n";
    std::cout << "  output.wav      Файл на выход\n";
    std::cout << "  input*.wav      Файлы на вход (WAV format, mono, 16-bit, 44100 Hz)\n";
    std::cout << ConverterFactory::getHelpText();
    std::cout << "\nПример конфиг файла:\n";
    std::cout << "  # Тишина первые 5 секунд\n";
    std::cout << "  mute 0 5\n";
    std::cout << "  # Замиксовать со вторым входным файлом начиная с 10 секунд\n";
    std::cout << "  mix $2 10\n";
    std::cout << "  # Добавить эхо с отставанием в 0.5 секунд и затуханием 70%\n";
    std::cout << "  echo 0.5 0.7\n";
}

void SoundProcessorApp::processAudio(
    const std::string& configFile,
    const std::string& outputFile,
    const std::vector<std::string>& inputFiles) {

    if (inputFiles.empty()) {
        throw ArgumentException("No input files specified");
    }

    WavStreamReader mainIn(inputFiles[0]);
    uint32_t sampleRate = mainIn.sampleRate();

    std::cout << "Main stream: " << inputFiles[0] << "\n";

    ConfigParser parser;
    auto configs = parser.parse(configFile);

    if (configs.empty()) {
        std::cout << "Warning: No converters specified in config file\n";
    }

    std::vector<std::unique_ptr<Converter>> chain;
    chain.reserve(configs.size());

    for (const auto& cfg : configs) {
        chain.push_back(ConverterFactory::create(
            cfg.name,
            cfg.params,
            inputFiles,
            sampleRate
        ));
    }

    WavStreamWriter out(outputFile, sampleRate);

    constexpr size_t bufferSize = 4096;
    std::vector<int16_t> buffer(bufferSize);

    size_t globalIndex = 0;

    while (true) {
        size_t got = mainIn.readSamples(buffer.data(), buffer.size());
        if (got == 0) break;

        for (size_t i = 0; i < got; ++i) {
            for (auto& c : chain) {
                buffer[i] = c->process(buffer[i], globalIndex);
            }
            ++globalIndex;
        }

        out.writeSamples(buffer.data(), got);
    }

    out.finalize();

    std::cout << "Successfully completed!\n";
}