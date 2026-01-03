#pragma once
#include <map>
#include <string>

class Config {
private:
    static std::map<std::string, double> values;

public:
    static void load(const std::string& filepath);
    static double get(const std::string& key, double defaultValue);
};
