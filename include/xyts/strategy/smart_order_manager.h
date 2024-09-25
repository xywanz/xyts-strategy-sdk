#pragma once

#include <unordered_map>

#include "xyts/core/contract_table.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

using SmartOrderId = uint64_t;

struct SmartOrderState {
  SmartOrderId smart_order_id;
  ContractPtr contract;
  Volume original_volume;
  Direction direction;
  OrderType order_type;
  double price;
  std::chrono::microseconds timeout;

  bool canceling = false;
  bool alive = true;

  Volume accum_trade_volume = 0;
  Volume canceled_volume = 0;
  Volume rejected_volume = 0;

  Volume current_trade_volume = 0;
  Volume current_trade_price = 0;

  std::unordered_map<ClientOrderId, OrderResponse> orders;
  std::size_t alive_order_count = 0;
};

using SmartOrderCallback = std::function<void(const SmartOrderState&)>;

class SmartOrderManager {
 public:
  explicit SmartOrderManager(StrategyContext* ctx);

  SmartOrderId SendOrder(SmartOrderCallback&& cb, ContractId contract_id, Volume volume,
                         Direction direction, OrderType type, double price,
                         std::chrono::microseconds timeout = kNeverTimeout);

  void CancelOrder(SmartOrderId smart_order_id);

  void OnOrder(const OrderResponse& order);

 private:
  struct SmartOrder {
    SmartOrderState state;
    SmartOrderCallback cb;
  };

  StrategyContext* ctx_;

  SmartOrderId next_smart_order_id_ = 1;
  std::unordered_map<SmartOrderId, SmartOrder> smart_orders_;
  std::unordered_map<ClientOrderId, SmartOrder*> coid_to_smart_orders_;
};

}  // namespace xyts::strategy
