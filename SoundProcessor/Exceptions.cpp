#include "Exceptions.h"

AudioException::AudioException(const std::string& message) 
    : message(message) {
}

const char* AudioException::what() const noexcept {
    return message.c_str();
}

int AudioException::getExitCode() const {
    return 99;
}

ArgumentException::ArgumentException(const std::string& message)
    : AudioException(message) {
}

int ArgumentException::getExitCode() const {
    return 1;
}

UnsupportedFormatException::UnsupportedFormatException(const std::string& message)
    : AudioException(message) {
}

int UnsupportedFormatException::getExitCode() const {
    return 2;
}

FileException::FileException(const std::string& message)
    : AudioException(message) {
}

int FileException::getExitCode() const {
    return 3;
}

ConfigException::ConfigException(const std::string& message)
    : AudioException(message) {
}

int ConfigException::getExitCode() const {
    return 4;
}

ConverterException::ConverterException(const std::string& message)
    : AudioException(message) {
}

int ConverterException::getExitCode() const {
    return 5;
}