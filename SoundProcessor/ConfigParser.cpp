#include "ConfigParser.h"
#include "Exceptions.h"
#include <fstream>
#include <sstream>

std::vector<ConverterConfig> ConfigParser::parse(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw ConfigException("Cannot open config file: " + filename);
    }
    
    std::vector<ConverterConfig> configs;
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);

        if (line.empty() || isComment(line)) {
            continue;
        }
        
        try {
            ConverterConfig config = parseLine(line);
            configs.push_back(config);
        } catch (const ConfigException& e) {
            throw ConfigException("Line " + std::to_string(lineNum) + ": " + e.what());
        }
    }
    
    file.close();
    return configs;
}

std::string ConfigParser::trim(const std::string& str) {
    const size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    const size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ConfigParser::isComment(const std::string& line) {
    if (line.empty()) return false;
    return line[0] == '#';
}

ConverterConfig ConfigParser::parseLine(const std::string& line) {
    std::istringstream iss(line);
    std::string name;
    
    if (!(iss >> name)) {
        throw ConfigException("Empty line or invalid format");
    }
    
    ConverterConfig config;
    config.name = name;
    
    std::string param;
    while (iss >> param) {
        config.params.push_back(param);
    }
    
    return config;
}