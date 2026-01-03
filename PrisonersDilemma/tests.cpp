#include <gtest/gtest.h>
#include "Game.h"
#include "Factory.h"
#include "Strategies.h"

// тесты матрицы

TEST(GameTest, Matrix_AllCooperate) {
    Game game;
    auto scores = game.getScores(Move::C, Move::C, Move::C);
    EXPECT_EQ(std::get<0>(scores), 7);
    EXPECT_EQ(std::get<1>(scores), 7);
    EXPECT_EQ(std::get<2>(scores), 7);
}

TEST(GameTest, Matrix_OneDefects_First) {
    Game game;
    auto scores = game.getScores(Move::D, Move::C, Move::C);
    EXPECT_EQ(std::get<0>(scores), 9);
    EXPECT_EQ(std::get<1>(scores), 3);
    EXPECT_EQ(std::get<2>(scores), 3);
}

TEST(GameTest, Matrix_OneDefects_Second) {
    Game game;
    auto scores = game.getScores(Move::C, Move::D, Move::C);
    EXPECT_EQ(std::get<0>(scores), 3);
    EXPECT_EQ(std::get<1>(scores), 9);
    EXPECT_EQ(std::get<2>(scores), 3);
}

TEST(GameTest, Matrix_OneDefects_Third) {
    Game game;
    auto scores = game.getScores(Move::C, Move::C, Move::D);
    EXPECT_EQ(std::get<0>(scores), 3);
    EXPECT_EQ(std::get<1>(scores), 3);
    EXPECT_EQ(std::get<2>(scores), 9);
}

TEST(GameTest, Matrix_TwoDefect) {
    Game game;
    auto scores = game.getScores(Move::C, Move::D, Move::D);
    EXPECT_EQ(std::get<0>(scores), 0);
    EXPECT_EQ(std::get<1>(scores), 5);
    EXPECT_EQ(std::get<2>(scores), 5);
}

TEST(GameTest, Matrix_AllDefect) {
    Game game;
    auto scores = game.getScores(Move::D, Move::D, Move::D);
    EXPECT_EQ(std::get<0>(scores), 1);
    EXPECT_EQ(std::get<1>(scores), 1);
    EXPECT_EQ(std::get<2>(scores), 1);
}

// тесты простых стратегий

TEST(StrategyTest, Cooperate_AlwaysC) {
    CooperateStrategy s;
    EXPECT_EQ(s.decide(), Move::C);
    s.update(Move::C, Move::D, Move::D);
    EXPECT_EQ(s.decide(), Move::C);
    s.reset();
    EXPECT_EQ(s.decide(), Move::C);
}

TEST(StrategyTest, Defect_AlwaysD) {
    DefectStrategy s;
    EXPECT_EQ(s.decide(), Move::D);
    s.update(Move::D, Move::C, Move::C);
    EXPECT_EQ(s.decide(), Move::D);
    s.reset();
    EXPECT_EQ(s.decide(), Move::D);
}

TEST(StrategyTest, Random_ReturnsCOrD) {
    RandomStrategy s;
    for (int i = 0; i < 20; ++i) {
        Move m = s.decide();
        EXPECT_TRUE(m == Move::C || m == Move::D);
    }
}

// тесты titfortat

TEST(StrategyTest, TitForTat_StartsWithC) {
    TitForTatStrategy s;
    EXPECT_EQ(s.decide(), Move::C);
}

TEST(StrategyTest, TitForTat_IfBothCooperate_StaysC) {
    TitForTatStrategy s;
    s.decide();
    s.update(Move::C, Move::C, Move::C);
    EXPECT_EQ(s.decide(), Move::C);
}

TEST(StrategyTest, TitForTat_IfOneDefects_BecomesD) {
    TitForTatStrategy s;
    s.decide();
    s.update(Move::C, Move::D, Move::C);
    EXPECT_EQ(s.decide(), Move::D);
}

TEST(StrategyTest, TitForTat_BothDefect_Vengeance) {
    TitForTatStrategy s;
    s.decide();
    s.update(Move::C, Move::D, Move::D);
    EXPECT_EQ(s.decide(), Move::D);
}

TEST(StrategyTest, TitForTat_Forgives) {
    TitForTatStrategy s;
    s.decide();
    s.update(Move::C, Move::D, Move::C);
    EXPECT_EQ(s.decide(), Move::D);
    s.update(Move::D, Move::C, Move::C);
    EXPECT_EQ(s.decide(), Move::C);
}

// тесты мстителя

TEST(StrategyTest, Revenge_FirstTurnIsC) {
    RevengeStrategy s;
    EXPECT_EQ(s.decide(), Move::C);
}

TEST(StrategyTest, Revenge_BothCooperateStaysC) {
    RevengeStrategy s;
    s.decide();
    s.update(Move::C, Move::C, Move::C);
    EXPECT_EQ(s.decide(), Move::C);
}

TEST(StrategyTest, Revenge_OneDefects_BecomesD) {
    RevengeStrategy s;
    s.decide();
    s.update(Move::C, Move::D, Move::C);
    EXPECT_EQ(s.decide(), Move::D);
}

TEST(StrategyTest, Revenge_MultipleRounds) {
    RevengeStrategy s;
    s.decide();
    s.update(Move::C, Move::C, Move::C);
    EXPECT_EQ(s.decide(), Move::C);
    s.update(Move::C, Move::D, Move::C);
    EXPECT_EQ(s.decide(), Move::D);
    s.update(Move::D, Move::C, Move::C);
    EXPECT_EQ(s.decide(), Move::C);
}

// тесты провокаций

TEST(StrategyTest, Provocation_StartsWithD) {
    ProvocationStrategy s;
    EXPECT_EQ(s.decide(), Move::D);
}

TEST(StrategyTest, Provocation_IfTheyRetaliateBacksOff) {
    ProvocationStrategy s;
    s.decide();
    s.update(Move::D, Move::D, Move::C);
    EXPECT_EQ(s.decide(), Move::C);
}

TEST(StrategyTest, Provocation_IfTheyTolerate_ContinueExploit) {
    ProvocationStrategy s;
    s.decide();
    s.update(Move::D, Move::C, Move::C);
    EXPECT_EQ(s.decide(), Move::D);
}

// тесты анализатора

TEST(StrategyTest, Analyzer_FirstTurnIsC) {
    AnalyzerStrategy s;
    EXPECT_EQ(s.decide(), Move::C);
}

TEST(StrategyTest, Analyzer_BothCooperate_StaysC) {
    AnalyzerStrategy s;
    s.decide();
    s.update(Move::C, Move::C, Move::C);
    EXPECT_EQ(s.decide(), Move::C);
}

TEST(StrategyTest, Analyzer_OneDefects_BecomesD) {
    AnalyzerStrategy s;
    s.decide();
    s.update(Move::C, Move::D, Move::C);
    EXPECT_EQ(s.decide(), Move::D);
}

TEST(StrategyTest, Analyzer_CountsDefects) {
    AnalyzerStrategy s;
    s.decide();
    s.update(Move::C, Move::C, Move::C);
    EXPECT_EQ(s.decide(), Move::C);
    s.update(Move::C, Move::D, Move::C);
    EXPECT_EQ(s.decide(), Move::D);
}

// тесты мета

TEST(StrategyTest, Meta_VotingMajority) {
    auto s1 = std::make_shared<CooperateStrategy>();
    auto s2 = std::make_shared<CooperateStrategy>();
    auto s3 = std::make_shared<DefectStrategy>();
    MetaStrategy meta(s1, s2, s3);
    EXPECT_EQ(meta.decide(), Move::C);
}

TEST(StrategyTest, Meta_VotingMinority) {
    auto s1 = std::make_shared<DefectStrategy>();
    auto s2 = std::make_shared<DefectStrategy>();
    auto s3 = std::make_shared<CooperateStrategy>();
    MetaStrategy meta(s1, s2, s3);
    EXPECT_EQ(meta.decide(), Move::D);
}

TEST(StrategyTest, Meta_UpdatePropagates) {
    auto s1 = std::make_shared<CooperateStrategy>();
    auto s2 = std::make_shared<DefectStrategy>();
    auto s3 = std::make_shared<RandomStrategy>();
    MetaStrategy meta(s1, s2, s3);
    meta.decide();
    meta.update(Move::C, Move::D, Move::C);
    EXPECT_NO_THROW(meta.decide());
}

TEST(StrategyTest, Meta_ResetWorks) {
    auto s1 = std::make_shared<CooperateStrategy>();
    auto s2 = std::make_shared<DefectStrategy>();
    auto s3 = std::make_shared<RandomStrategy>();
    MetaStrategy meta(s1, s2, s3);
    meta.decide();
    meta.update(Move::C, Move::D, Move::D);
    meta.reset();
    EXPECT_NO_THROW(meta.decide());
}

// тесты фабрики

TEST(FactoryTest, CreateCooperate) {
    auto& f = StrategyFactory::getInstance();
    auto s = f.create("cooperate");
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->decide(), Move::C);
}

TEST(FactoryTest, CreateDefect) {
    auto& f = StrategyFactory::getInstance();
    auto s = f.create("defect");
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->decide(), Move::D);
}

TEST(FactoryTest, CreateRandom) {
    auto& f = StrategyFactory::getInstance();
    auto s = f.create("random");
    ASSERT_NE(s, nullptr);
    Move m = s->decide();
    EXPECT_TRUE(m == Move::C || m == Move::D);
}

TEST(FactoryTest, CreateTitForTat) {
    auto& f = StrategyFactory::getInstance();
    auto s = f.create("titfortat");
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->decide(), Move::C);
}

TEST(FactoryTest, CreateRevenge) {
    auto& f = StrategyFactory::getInstance();
    auto s = f.create("revenge");
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->decide(), Move::C);
}

TEST(FactoryTest, CreateProvocator) {
    auto& f = StrategyFactory::getInstance();
    auto s = f.create("provocator");
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->decide(), Move::D);
}

TEST(FactoryTest, CreateAnalyzer) {
    auto& f = StrategyFactory::getInstance();
    auto s = f.create("analyzer");
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->decide(), Move::C);
}

TEST(FactoryTest, CreateMeta) {
    auto& f = StrategyFactory::getInstance();
    auto s = f.create("meta");
    ASSERT_NE(s, nullptr);
    Move m = s->decide();
    EXPECT_TRUE(m == Move::C || m == Move::D);
}

TEST(FactoryTest, CreateUnknown) {
    auto& f = StrategyFactory::getInstance();
    auto s = f.create("unknown_strategy_xyz");
    EXPECT_EQ(s, nullptr);
}

TEST(FactoryTest, ListAllStrategies) {
    auto& f = StrategyFactory::getInstance();
    auto list = f.getAvailableStrategies();
    EXPECT_EQ(list.size(), 8);
    EXPECT_TRUE(std::find(list.begin(), list.end(), "cooperate") != list.end());
    EXPECT_TRUE(std::find(list.begin(), list.end(), "defect") != list.end());
    EXPECT_TRUE(std::find(list.begin(), list.end(), "meta") != list.end());
}

// интеграционные тесты

TEST(GameIntegration, SimpleGameWithThreeStrategies) {
    Game game;
    std::vector<std::string> strategies = {"cooperate", "defect", "random"};
    EXPECT_NO_THROW(game.play(strategies, 5, "fast"));
}

TEST(GameIntegration, TournamentWithFourStrategies) {
    Game game;
    std::vector<std::string> strategies = {"cooperate", "defect", "random", "titfortat"};
    EXPECT_NO_THROW(game.tournament(strategies, 10));
}
