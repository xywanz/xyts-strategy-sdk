#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "xyts/core/contract_table.h"
#include "xyts/core/market_data.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

using BarCallback = std::function<void(const BarData&)>;

class BarGenerator {
 public:
  explicit BarGenerator(StrategyContext* ctx);

  ~BarGenerator();

  void AddBarPeriod(const std::vector<std::string>& patterns, std::chrono::seconds period,
                    BarCallback&& cb);

  void UpdateBar(const DepthData& depth);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
