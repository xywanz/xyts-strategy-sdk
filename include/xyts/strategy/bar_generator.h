#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "xyts/core/contract_table.h"
#include "xyts/core/market_data.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

class BarGenerator {
 public:
  using Callback = std::function<void(const BarData&)>;

  explicit BarGenerator(StrategyContext* ctx, const std::vector<ContractPtr>& contracts,
                        int64_t period_sec, Callback&& cb);

  ~BarGenerator();

  void OnTick(const TickData& tick);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
