#pragma once

#include <chrono>
#include <memory>
#include <string>

#include "xyts/base/trade_msg.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

class OrderManager {
 public:
  explicit OrderManager(StrategyContext* ctx, const std::string& name = "");

  ~OrderManager();

  void SetMarket(uint32_t ticker_id, int bid_volume, double bid_price, int ask_volume,
                 double ask_price, std::chrono::microseconds timeout = kDefaultTimeout,
                 Offset bid_offset = Offset::kAuto,
                 Offset ask_offset = Offset::kAuto) noexcept(false);

  bool AnyOrderInTheMarket(uint32_t ticker_id) const;
  bool AnyBidOrderInTheMarket(uint32_t ticker_id) const;
  bool AnyAskOrderInTheMarket(uint32_t ticker_id) const;

  void OnOrder(const OrderResponse& order);
  void OnTrade(const OrderResponse& trade);

  void EnableLog();
  void DisableLog();

  static constexpr std::chrono::microseconds kDefaultTimeout =
      std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::days{1});

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
