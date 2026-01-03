#pragma once
#include <memory>
#include <string>

enum class Move { C, D };

class IStrategy {
public:
    virtual ~IStrategy() = default;

    virtual Move decide() = 0;

    virtual void update(Move my_move, Move opp1_move, Move opp2_move) = 0;

    virtual void reset() = 0;
};

using StrategyPtr = std::shared_ptr<IStrategy>;
