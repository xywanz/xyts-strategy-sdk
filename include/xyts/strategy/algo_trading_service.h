#pragma once

#include <memory>

#include "xyts/core/market_data.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

using AlgoOrderId = uint64_t;

class AlgoTradingService {
 public:
  explicit AlgoTradingService(StrategyContext* ctx);

  ~AlgoTradingService();

  AlgoOrderId AddAlgoOrder(const nlohmann::json& algo_params);

  void CancelAlgoOrder(AlgoOrderId algo_order_id);

  void OnDepth(const DepthData& depth);

  void OnOrder(const OrderResponse& order);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
