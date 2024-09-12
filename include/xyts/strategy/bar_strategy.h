#pragma once

#include "xyts/strategy/bar_generator.h"
#include "xyts/strategy/strategy.h"

namespace xyts::strategy {

class BarStrategy : public Strategy {
 public:
  explicit BarStrategy(StrategyContext* ctx);

  void SubscribeBarData(const std::vector<std::string>& patterns, std::chrono::seconds period);

  virtual void OnBar(const BarData& bar) {}

 protected:
  StrategyContext* ctx_;

 private:
  void OnDepth(const DepthData& depth) final;

  BarGenerator bar_generator_;
};

}  // namespace xyts::strategy
