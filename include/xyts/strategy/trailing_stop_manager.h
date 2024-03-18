#pragma once

#include <memory>

#include "xyts/core/market_data.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

using TrailingStopTaskId = uint64_t;

class TrailingStopManager {
 public:
  explicit TrailingStopManager(StrategyContext* ctx);

  ~TrailingStopManager();

  TrailingStopTaskId AddTrailingStop(ContractId contract_id, Direction position_direction,
                                     Volume volume, double base_price, double stop_distance);

  void CancelTrailingStop(TrailingStopTaskId id);

  bool IsRunning(TrailingStopTaskId id) const;

  void OnTick(const TickData& tick);

  void OnOrder(const OrderResponse& order);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
