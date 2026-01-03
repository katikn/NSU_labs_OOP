#include "Game.h"
#include "Factory.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>

Game::Game() {
    matrix_[{Move::C, Move::C, Move::C}] = {7, 7, 7};
    matrix_[{Move::C, Move::C, Move::D}] = {3, 3, 9};
    matrix_[{Move::C, Move::D, Move::C}] = {3, 9, 3};
    matrix_[{Move::D, Move::C, Move::C}] = {9, 3, 3};
    matrix_[{Move::C, Move::D, Move::D}] = {0, 5, 5};
    matrix_[{Move::D, Move::C, Move::D}] = {5, 0, 5};
    matrix_[{Move::D, Move::D, Move::C}] = {5, 5, 0};
    matrix_[{Move::D, Move::D, Move::D}] = {1, 1, 1};
}

Game::Scores Game::getScores(Move m1, Move m2, Move m3) {
    return matrix_[{m1, m2, m3}];
}

void Game::printTurn(int step, Move m1, Move m2, Move m3, Scores s, Scores total) {
    char c1 = (m1 == Move::C ? 'C' : 'D');
    char c2 = (m2 == Move::C ? 'C' : 'D');
    char c3 = (m3 == Move::C ? 'C' : 'D');

    std::cout << "Turn " << std::setw(3) << step << ": "
              << c1 << " " << c2 << " " << c3
              << " => Points: "
              << std::get<0>(s) << " " << std::get<1>(s) << " " << std::get<2>(s)
              << " | Total: "
              << std::get<0>(total) << " " << std::get<1>(total) << " " << std::get<2>(total)
              << std::endl;
}

void Game::play(const std::vector<std::string>& names, int steps, const std::string& mode) {
    auto& f = StrategyFactory::getInstance();
    auto s1 = f.create(names[0]);
    auto s2 = f.create(names[1]);
    auto s3 = f.create(names[2]);

    if (!s1 || !s2 || !s3) {
        std::cerr << "Error: Could not create strategies." << std::endl;
        return;
    }

    s1->reset(); s2->reset(); s3->reset();

    int total1 = 0, total2 = 0, total3 = 0;

    for (int i = 1; i <= steps; ++i) {
        if (mode == "detailed") {
            std::cout << "Press Enter for next step...";
            std::cin.get();
        }

        Move m1 = s1->decide();
        Move m2 = s2->decide();
        Move m3 = s3->decide();

        auto currentScores = getScores(m1, m2, m3);
        total1 += std::get<0>(currentScores);
        total2 += std::get<1>(currentScores);
        total3 += std::get<2>(currentScores);

        s1->update(m1, m2, m3);
        s2->update(m2, m1, m3);
        s3->update(m3, m1, m2);

        if (mode == "detailed") {
            printTurn(i, m1, m2, m3, currentScores, {total1, total2, total3});
        }
    }

    std::cout << "Game Result: "
              << names[0] << ": " << total1 << ", "
              << names[1] << ": " << total2 << ", "
              << names[2] << ": " << total3 << std::endl;
}

void Game::tournament(const std::vector<std::string>& names, int steps) {
    int n = static_cast<int>(names.size());
    if (n < 3) {
        throw std::runtime_error("Tournament requires at least 3 strategies");
    }

    std::vector<int> totalScores(n, 0);

    auto& f = StrategyFactory::getInstance();

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            for (int k = j + 1; k < n; ++k) {

                auto s1 = f.create(names[i]);
                auto s2 = f.create(names[j]);
                auto s3 = f.create(names[k]);

                if (!s1 || !s2 || !s3) {
                    throw std::runtime_error("Failed to create strategy in tournament");
                }

                s1->reset(); s2->reset(); s3->reset();

                int score1 = 0, score2 = 0, score3 = 0;

                for (int step = 0; step < steps; ++step) {
                    Move m1 = s1->decide();
                    Move m2 = s2->decide();
                    Move m3 = s3->decide();

                    auto sc = getScores(m1, m2, m3);
                    score1 += std::get<0>(sc);
                    score2 += std::get<1>(sc);
                    score3 += std::get<2>(sc);

                    s1->update(m1, m2, m3);
                    s2->update(m2, m1, m3);
                    s3->update(m3, m1, m2);
                }

                totalScores[i] += score1;
                totalScores[j] += score2;
                totalScores[k] += score3;
            }
        }
    }

    std::cout << "=== Tournament Results ===" << std::endl;
    for (int idx = 0; idx < n; ++idx) {
        std::cout << "Player " << (idx + 1) << " (" << names[idx]
                  << "): " << totalScores[idx] << std::endl;
    }
}

void Game::loadMatrixFromFile(const std::string& filepath) {
    std::ifstream file;
    file.exceptions(std::ifstream::badbit);

    try {
        file.open(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("File not found: " + filepath);
        }

        matrix_.clear();

        std::string line;
        int lineNumber = 0;
        int validLines = 0;

        while (std::getline(file, line)) {
            ++lineNumber;
            if (line.empty()) continue;

            std::stringstream ss(line);
            char c1, c2, c3;
            int s1, s2, s3;

            if (!(ss >> c1 >> c2 >> c3 >> s1 >> s2 >> s3)) {
                throw std::runtime_error(
                    "Invalid line format in matrix file at line " + std::to_string(lineNumber)
                );
            }

            std::string extra;
            if (ss >> extra) {
                throw std::runtime_error(
                    "Extra data in matrix file at line " + std::to_string(lineNumber)
                );
            }

            auto toMove = [](char c) {
                if (c == 'C' || c == 'c') return Move::C;
                if (c == 'D' || c == 'd') return Move::D;
                throw std::runtime_error("Invalid move character in matrix file");
            };

            Move m1 = toMove(c1);
            Move m2 = toMove(c2);
            Move m3 = toMove(c3);

            auto key = std::make_tuple(m1, m2, m3);
            auto value = std::make_tuple(s1, s2, s3);

            auto [it, inserted] = matrix_.insert({key, value});
            if (!inserted) {
                throw std::runtime_error(
                    "Duplicate combination in matrix file at line " + std::to_string(lineNumber)
                );
            }

            ++validLines;
        }
        if (validLines != 8) {
            throw std::runtime_error(
                "Matrix file must contain exactly 8 non-empty valid lines, got " +
                std::to_string(validLines)
            );
        }

    } catch (const std::exception& e) {
        throw std::runtime_error(
            std::string("Failed to read matrix file: ") + filepath + " (" + e.what() + ")"
        );
    }
}
// уникальные строки (комбинации) и лишние элементы в строке
