#pragma once
#include "Strategy.h"
#include <map>
#include <tuple>
#include <vector>
#include <string>

class Game {
public:
    using Scores = std::tuple<int, int, int>;
    using Moves = std::tuple<Move, Move, Move>;

    Game();

    void play(const std::vector<std::string>& names, int steps, const std::string& mode);
    void tournament(const std::vector<std::string>& names, int steps);
    void loadMatrixFromFile(const std::string& filepath);
    Scores getScores(Move m1, Move m2, Move m3);

private:
    std::map<Moves, Scores> matrix_;
    void printTurn(int step, Move m1, Move m2, Move m3, Scores s, Scores total);
};
