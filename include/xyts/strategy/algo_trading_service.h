#pragma once

#include <memory>

#include "xyts/base/market_data.h"
#include "xyts/base/trade_msg.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

using AlgoOrderId = uint64_t;

class AlgoTradingService {
 public:
  explicit AlgoTradingService(StrategyContext* ctx);

  ~AlgoTradingService();

  AlgoOrderId AddAlgoOrder(const nlohmann::json& algo_params);

  void CancelAlgoOrder(AlgoOrderId algo_order_id);

  void OnTick(const TickData& tick);

  void OnOrder(const OrderResponse& order);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
