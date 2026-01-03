#pragma once
#include <string>
#include <vector>
#include <cstdint>

class SoundProcessorApp {
public:
    int run(int argc, char* argv[]);
    
private:
    void printHelp();

    void processAudio(
        const std::string& configFile,
        const std::string& outputFile,
        const std::vector<std::string>& inputFiles
    );

    std::vector<std::vector<int16_t>> loadInputFiles(
        const std::vector<std::string>& filenames
    );
};