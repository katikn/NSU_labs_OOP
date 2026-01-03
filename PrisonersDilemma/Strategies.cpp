#include "Strategies.h"
#include "Config.h"
#include <random>

// сотрудничать
Move CooperateStrategy::decide() {
    return Move::C;
}

// предать
Move DefectStrategy::decide() {
    return Move::D;
}

// рандом
Move RandomStrategy::decide() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    return dis(gen) == 0 ? Move::C : Move::D;
}

// спровоцировали ранее - предаю, иначе - сотрудничаю
Move TitForTatStrategy::decide() {
    if (provoked) return Move::D;
    return Move::C;
}

void TitForTatStrategy::update(Move my, Move opp1, Move opp2) {
    provoked = opp1 == Move::D || opp2 == Move::D;
}

void TitForTatStrategy::reset() {
    provoked = false;
}

// метастратегия
MetaStrategy::MetaStrategy(StrategyPtr s1, StrategyPtr s2, StrategyPtr s3) {
    slaves.push_back(s1);
    slaves.push_back(s2);
    slaves.push_back(s3);
}

Move MetaStrategy::decide() {
    int c_votes = 0;
    int d_votes = 0;

    for (auto& s : slaves) {
        if (s->decide() == Move::C) c_votes++;
        else d_votes++;
    }

    return (c_votes > d_votes) ? Move::C : Move::D;
}

void MetaStrategy::update(Move my, Move opp1, Move opp2) {
    for (auto& s : slaves) {
        s->update(my, opp1, opp2);
    }
}

void MetaStrategy::reset() {
    for (auto& s : slaves) {
        s->reset();
    }
}

// обобщить стратегии revenge и provocation, также ненужную историю-массив убрать
// одинаковый класс с параметром или наследование какое-нибудь

// мститель + провокатор
Move ReactiveStrategy::decide() {
    if (isFirstTurn()) {
        return (mode_ == Mode::Provocation ? Move::D : Move::C);
    }

    Move last1 = lastOpp1;
    Move last2 = lastOpp2;

    // мститель
    if (mode_ == Mode::Revenge) {
        if (last1 == Move::C && last2 == Move::C) return Move::C;
        return Move::D;
    }
    // провокатор
    if (last1 == Move::D || last2 == Move::D) return Move::C;
    return Move::D;
}

// аналитик
Move AnalyzerStrategy::decide() {
    if (isFirstTurn()) return Move::C;

    // сколько раз каждый враг предавал
    int d1 = 0;
    int d2 = 0;
    for (const auto m : opp1Moves) if (m == Move::D) d1++;
    for (const auto m : opp2Moves) if (m == Move::D) d2++;

    double percent1 = static_cast<double>(d1) / opp1Moves.size();
    double percent2 = static_cast<double>(d2) / opp2Moves.size();

    if (percent1 > analyzer_percent || percent2 > analyzer_percent) return Move::D;
    return Move::C;
}
