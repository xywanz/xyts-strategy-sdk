#pragma once

#include <memory>

#include "xyts/core/market_data.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

class TargetPositionManager {
 public:
  explicit TargetPositionManager(StrategyContext* ctx);

  ~TargetPositionManager();

  void SetTargetPosition(ContractId contract_id, Volume target_volume,
                         OrderType order_type = OrderType::kLimit, bool aggressive = true,
                         bool exec_right_now = false);

  void StartTarget(ContractId contract_id, bool exec_right_now = false);

  void StopTarget(ContractId contract_id);

  void OnDepth(const DepthData& depth);

  void OnOrder(const OrderResponse& order);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
