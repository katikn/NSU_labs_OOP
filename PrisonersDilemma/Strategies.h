#pragma once
#include "Strategy.h"
#include "Config.h"
#include <vector>

class StrategyWithHistory : public IStrategy {
protected:
    std::vector<Move> myMoves;
    std::vector<Move> opp1Moves;
    std::vector<Move> opp2Moves;
    double analyzer_percent = Config::get("ANALYZER_THRESHOLD", 0.1);

public:
    void update(Move my, Move opp1, Move opp2) override {
        myMoves.push_back(my);
        opp1Moves.push_back(opp1);
        opp2Moves.push_back(opp2);
    }

    void reset() override {
        myMoves.clear();
        opp1Moves.clear();
        opp2Moves.clear();
    }

    bool isFirstTurn() const { return myMoves.empty(); }
};

class StrategyWithLastMoves : public IStrategy {
protected:
    bool hasLast = false;
    Move lastOpp1{};
    Move lastOpp2{};

public:
    void update(Move my, Move opp1, Move opp2) override {
        hasLast = true;
        lastOpp1 = opp1;
        lastOpp2 = opp2;
    }

    void reset() override {
        hasLast = false;
    }

    bool isFirstTurn() const { return !hasLast; }
};

class CooperateStrategy : public IStrategy {
public:
    Move decide() override;
    void update(Move, Move, Move) override {}
    void reset() override {}
};

class DefectStrategy : public IStrategy {
public:
    Move decide() override;
    void update(Move, Move, Move) override {}
    void reset() override {}
};

class RandomStrategy : public IStrategy {
public:
    Move decide() override;
    void update(Move, Move, Move) override {}
    void reset() override {}
};

class TitForTatStrategy : public IStrategy {
    bool provoked = false;
public:
    Move decide() override;
    void update(Move my, Move opp1, Move opp2) override;
    void reset() override;
};

class MetaStrategy : public IStrategy {
    std::vector<std::shared_ptr<IStrategy>> slaves;
public:
    MetaStrategy(std::shared_ptr<IStrategy> s1, std::shared_ptr<IStrategy> s2, std::shared_ptr<IStrategy> s3);

    Move decide() override;
    void update(Move my, Move opp1, Move opp2) override;
    void reset() override;
};

class ReactiveStrategy : public StrategyWithLastMoves {
public:
    enum class Mode { Revenge, Provocation };

    explicit ReactiveStrategy(Mode mode) : mode_(mode) {}

    Move decide() override;

private:
    Mode mode_;
};

class RevengeStrategy : public ReactiveStrategy {
public:
    RevengeStrategy() : ReactiveStrategy(Mode::Revenge) {}
};

class ProvocationStrategy : public ReactiveStrategy {
public:
    ProvocationStrategy() : ReactiveStrategy(Mode::Provocation) {}
};

class AnalyzerStrategy : public StrategyWithHistory {
public:
    Move decide() override;
};
