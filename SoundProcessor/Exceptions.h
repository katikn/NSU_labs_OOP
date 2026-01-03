#pragma once
#include <exception>
#include <string>

class AudioException : public std::exception {
public:
    explicit AudioException(const std::string& message);
    ~AudioException() override = default;
    
    const char* what() const noexcept override;
    virtual int getExitCode() const;
    
protected:
    std::string message;
};

class ArgumentException : public AudioException {
public:
    explicit ArgumentException(const std::string& message);
    int getExitCode() const override;
};

class UnsupportedFormatException : public AudioException {
public:
    explicit UnsupportedFormatException(const std::string& message);
    int getExitCode() const override;
};

class FileException : public AudioException {
public:
    explicit FileException(const std::string& message);
    int getExitCode() const override;
};

class ConfigException : public AudioException {
public:
    explicit ConfigException(const std::string& message);
    int getExitCode() const override;
};

class ConverterException : public AudioException {
public:
    explicit ConverterException(const std::string& message);
    int getExitCode() const override;
};