#pragma once

#include <chrono>
#include <memory>
#include <string>

#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

class OrderManager {
 public:
  explicit OrderManager(StrategyContext* ctx, const std::string& name = "");

  ~OrderManager();

  void PlaceOrder(ContractId contract_id, Volume bid_volume, double bid_price, Volume ask_volume,
                  double ask_price, std::chrono::microseconds timeout = kDefaultTimeout,
                  Offset bid_offset = Offset::kAuto,
                  Offset ask_offset = Offset::kAuto) noexcept(false);

  bool AnyOrderInTheMarket(ContractId contract_id) const;
  bool AnyBidOrderInTheMarket(ContractId contract_id) const;
  bool AnyAskOrderInTheMarket(ContractId contract_id) const;

  void OnOrder(const OrderResponse& order);

  void EnableLog();
  void DisableLog();

  static constexpr std::chrono::microseconds kDefaultTimeout = std::chrono::days{1};

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
