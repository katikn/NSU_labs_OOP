#include "Factory.h"
#include "Strategies.h"
#include <utility>

StrategyFactory& StrategyFactory::getInstance() {
    static StrategyFactory instance;
    return instance;
}

StrategyFactory::StrategyFactory() {
    registerStrategy("cooperate", []() { return std::make_shared<CooperateStrategy>(); });
    registerStrategy("defect", []() { return std::make_shared<DefectStrategy>(); });
    registerStrategy("random", []() { return std::make_shared<RandomStrategy>(); });
    registerStrategy("titfortat", []() { return std::make_shared<TitForTatStrategy>(); });
    registerStrategy("revenge", []() { return std::make_shared<RevengeStrategy>(); });
    registerStrategy("provocator", []() { return std::make_shared<ProvocationStrategy>(); });
    registerStrategy("analyzer", []() { return std::make_shared<AnalyzerStrategy>(); });

    registerStrategy("meta", []() {
        return std::make_shared<MetaStrategy>(
                std::make_shared<CooperateStrategy>(),
                std::make_shared<DefectStrategy>(),
                std::make_shared<RandomStrategy>()
        );
    });
}

void StrategyFactory::registerStrategy(const std::string& name, Creator creator) {
    creators_[name] = std::move(creator);
}

std::shared_ptr<IStrategy> StrategyFactory::create(const std::string& name) {
    if (creators_.find(name) != creators_.end()) {
        return creators_[name]();
    }
    return nullptr;
}

std::vector<std::string> StrategyFactory::getAvailableStrategies() const {
    std::vector<std::string> names;
    names.reserve(creators_.size());
    for (const auto& pair : creators_) {
        names.push_back(pair.first);
    }
    return names;
}
