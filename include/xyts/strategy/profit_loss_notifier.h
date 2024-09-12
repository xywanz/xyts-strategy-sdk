#pragma once

#include <functional>

#include "xyts/core/market_data.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/pnl_calculator.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

class ProfitLossNotifier {
 public:
  using Callback = std::function<void(double)>;

  explicit ProfitLossNotifier(StrategyContext* ctx,
                              double stop_loss = std::numeric_limits<double>::quiet_NaN(),
                              Callback&& stop_loss_cb = nullptr,
                              double take_profit = std::numeric_limits<double>::quiet_NaN(),
                              Callback&& take_profit_cb = nullptr,
                              std::chrono::seconds check_interval = std::chrono::seconds{10});

  ~ProfitLossNotifier();

  void OnDepth(const DepthData& depth);

  void OnOrderResponse(const OrderResponse& order);

  double pnl() const { return pnl_calculator_.CalculatePnl(); }

 private:
  void CheckPnl() const;

  StrategyContext* ctx_;
  PnlCalculator pnl_calculator_;
  double stop_loss_;
  double take_profit_;
  Callback stop_loss_cb_;
  Callback take_profit_cb_;
  std::chrono::seconds check_interval_;
  EventId timer_id_ = -1;
};

}  // namespace xyts::strategy
