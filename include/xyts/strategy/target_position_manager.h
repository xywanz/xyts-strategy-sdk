#pragma once

#include <memory>

#include "xyts/base/market_data.h"
#include "xyts/base/trade_msg.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

class TargetPositionManager {
 public:
  explicit TargetPositionManager(StrategyContext* ctx);

  ~TargetPositionManager();

  void SetTargetPosition(uint32_t ticker_id, int target_volume, bool aggressive = true,
                         bool exec_right_now = false);

  void StartTarget(uint32_t ticker_id, bool exec_right_now = false);

  void StopTarget(uint32_t ticker_id);

  void OnTick(const TickData& tick);

  void OnOrder(const OrderResponse& order);

  void OnPosition(const LogicalPositionData& pos);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
