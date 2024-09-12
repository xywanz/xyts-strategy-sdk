#pragma once

#include <memory>

#include "xyts/core/market_data.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_context.h"
#include "xyts/utils/math_utils.h"

namespace xyts::strategy {

using TrailingStopTaskId = uint64_t;

class TrailingStopManager {
 public:
  TrailingStopManager(StrategyContext* ctx, std::string_view name, bool persist = true);

  ~TrailingStopManager();

  TrailingStopTaskId AddTrailingStop(ContractId contract_id, Direction position_direction,
                                     Volume volume, double stop_distance,
                                     double base_price = NaNPrice());

  TrailingStopTaskId AddStop(ContractId contract_id, Direction position_direction, Volume volume,
                             double stop_price);

  void Cancel(TrailingStopTaskId id);

  bool IsRunning(TrailingStopTaskId id) const;

  void OnDepth(const DepthData& depth);

  void OnOrder(const OrderResponse& order);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
