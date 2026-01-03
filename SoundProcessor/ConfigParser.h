#pragma once
#include <string>
#include <vector>

struct ConverterConfig {
    std::string name;
    std::vector<std::string> params;
};

class ConfigParser {
public:
    std::vector<ConverterConfig> parse(const std::string& filename);
    
private:
    std::string trim(const std::string& str);

    bool isComment(const std::string& line);

    ConverterConfig parseLine(const std::string& line);
};