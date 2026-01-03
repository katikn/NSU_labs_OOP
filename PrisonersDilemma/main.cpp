#include <iostream>
#include <string>
#include <vector>
#include "Config.h"
#include "Factory.h"
#include "Game.h"

int main(int argc, char* argv[]) {
    Config::load("config.txt");
    std::string mode = "fast";
    int steps = 10;
    std::string matrixFile;
    std::vector<std::string> strategyNames;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--mode=") == 0) {
            mode = arg.substr(7);
        } else if (arg.find("--steps=") == 0) {
            steps = std::stoi(arg.substr(8));
        } else if (arg.find("--matrix=") == 0) {
            matrixFile = arg.substr(9);
        } else {
            strategyNames.push_back(arg);
        }
    }

    if (strategyNames.size() < 3) {
        std::cerr << "Error: Need at least 3 strategies." << std::endl;
        std::cerr << "Available strategies: ";
        for (const auto& name : StrategyFactory::getInstance().getAvailableStrategies()) {
            std::cerr << name << " ";
        }
        std::cerr << std::endl;
        return 1;
    }

    Game game;

    if (!matrixFile.empty()) {
        try {
            game.loadMatrixFromFile(matrixFile);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }

    if (mode == "tournament") {
        game.tournament(strategyNames, steps);
    } else {
        std::vector<std::string> players = {strategyNames[0], strategyNames[1], strategyNames[2]};
        game.play(players, steps, mode);
    }

    return 0;
}
