#include "SoundProcessorApp.h"
#include "Exceptions.h"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        SoundProcessorApp app;
        return app.run(argc, argv);
    } catch (const AudioException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return e.getExitCode();
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 99;
    }
}