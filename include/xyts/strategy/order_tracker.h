#pragma once

#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

class OrderTracker {
 public:
  using OrderCallback = std::function<void(const OrderResponse&)>;

  explicit OrderTracker(StrategyContext* ctx);

  ClientOrderId SendOrder(OrderCallback&& cb, ContractId contract_id, Volume volume,
                          Direction direction, PositionEffect position_effect, OrderType type,
                          double price,
                          std::chrono::microseconds timeout = StrategyContext::kNeverTimeout);

  ClientOrderId Buy(OrderCallback&& cb, ContractId contract_id, Volume volume, OrderType type,
                    double price,
                    std::chrono::microseconds timeout = StrategyContext::kNeverTimeout) {
    return SendOrder(std::move(cb), contract_id, volume, Direction::kBuy, PositionEffect::kAuto,
                     type, price, timeout);
  }

  ClientOrderId Sell(OrderCallback&& cb, ContractId contract_id, Volume volume, OrderType type,
                     double price,
                     std::chrono::microseconds timeout = StrategyContext::kNeverTimeout) {
    return SendOrder(std::move(cb), contract_id, volume, Direction::kSell, PositionEffect::kAuto,
                     type, price, timeout);
  }

  void CancelOrder(ClientOrderId client_order_id);

  void ResetOrderTimeout(ClientOrderId client_order_id, std::chrono::microseconds timeout);

  const OrderResponse* QueryOrder(ClientOrderId client_order_id);

  void OnOrder(const OrderResponse& order);

  void ClearCompletedOrders();

 private:
  struct OrderInfo {
    OrderResponse state;
    OrderCallback cb;
  };

  StrategyContext* ctx_;
  std::unordered_map<ClientOrderId, OrderInfo> orders_;
};

}  // namespace xyts::strategy
