#pragma once
#include "Strategy.h"
#include <map>
#include <functional>
#include <string>
#include <vector>
#include <memory>

class StrategyFactory {
public:
    using Creator = std::function<std::shared_ptr<IStrategy>()>;

    static StrategyFactory& getInstance();

    void registerStrategy(const std::string& name, Creator creator);

    std::shared_ptr<IStrategy> create(const std::string& name);

    std::vector<std::string> getAvailableStrategies() const;

private:
    StrategyFactory();
    std::map<std::string, Creator> creators_;
};
