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

  // 同时挂出买单和卖单，在挂出订单前，会先检查之前的挂单是否完成，如果已完成，则立即挂出新订单，
  // 若未完成，则分为三种情况：
  // 情况1：原挂单和当前挂单的价格相同，原挂单的未完成数量小于当前挂单量。则立即挂出新订单补齐缺失的单量即可
  // 情况2：原挂单和当前挂单的价格相同，原挂单的未完成数量大于当前挂单量。按照挂单顺序先撤销后挂出的订单，直到
  //       挂单量小于等于新请求的挂单量为止，撤完后如果等于则不进行任何操作，如小于则等待订单确认被撤销后，
  //       补齐缺失的单量
  // 情况3：原挂单和当前挂单的价格不同。撤销原来所有的挂单，并等待订单全部撤销后，挂出新的订单
  void PlaceOrder(ContractId contract_id, Volume bid_volume, double bid_price, Volume ask_volume,
                  double ask_price, std::chrono::microseconds timeout = kDefaultTimeout,
                  PositionEffect bid_position_effect = PositionEffect::kAuto,
                  PositionEffect ask_position_effect = PositionEffect::kAuto) noexcept(false);

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
