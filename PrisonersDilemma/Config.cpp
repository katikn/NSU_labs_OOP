#include "Config.h"
#include <fstream>
#include <sstream>

std::map<std::string, double> Config::values;

void Config::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return;

    std::string key;
    double value;
    while (file >> key >> value) {
        if (key[0] != '#') {
            values[key] = value;
        }
    }
}

double Config::get(const std::string& key, double defaultValue) {
    auto it = values.find(key);
    return (it != values.end()) ? it->second : defaultValue;
}
