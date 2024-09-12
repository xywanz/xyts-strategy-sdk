#pragma once

#include <memory>
#include <optional>

#include "xyts/core/contract_table.h"
#include "xyts/core/market_data.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

using ConditionOrderId = uint64_t;

class OrderExecMethod {
 public:
  enum class Type {
    kUserDefinedPrice,
    kConsiderationPrice,
  };

  static OrderExecMethod UserDefinedPrice(double price) {
    return OrderExecMethod{Type::kUserDefinedPrice, price};
  }

  static OrderExecMethod ConsiderationPrice() { return OrderExecMethod{Type::kConsiderationPrice}; }

  Type type() const { return type_; }

  double price() const { return price_; }

 private:
  explicit OrderExecMethod(Type type, double price = std::numeric_limits<double>::quiet_NaN())
      : type_(type), price_(price) {}

  Type type_;
  double price_;
};

class ConditionOrder {
 public:
  ConditionOrder(ContractId contract_id, Direction direction, PositionEffect position_effect,
                 Volume volume, double trigger_price,
                 OrderExecMethod exec_method = OrderExecMethod::ConsiderationPrice());

  ConditionOrder(const ConditionOrder&) = default;

  ContractPtr contract() const { return contract_; }

  Direction direction() const { return direction_; }

  PositionEffect position_effect() const { return position_effect_; }

  Volume volume() const { return volume_; }

  double trigger_price() const { return trigger_price_; }

  OrderExecMethod exec_method() const { return exec_method_; }

 private:
  ContractPtr contract_;
  Direction direction_;
  PositionEffect position_effect_;
  Volume volume_;
  double trigger_price_;
  OrderExecMethod exec_method_;
};

struct ConditionOrderExecutionDetails {
  ConditionOrderExecutionDetails(const ConditionOrder& order, ConditionOrderId id)
      : condition_order(order), condition_order_id(id) {}

  ConditionOrder condition_order;
  ConditionOrderId condition_order_id;
  Volume accum_trade_volume = 0;
  double accum_trade_amount = 0;
  bool running = true;
  bool waiting_for_stop = false;

  std::unordered_map<ClientOrderId, OrderResponse> orders{};
  std::size_t active_order_count = 0;
  Volume active_volume = 0;
};

class ConditionOrderManager {
 public:
  explicit ConditionOrderManager(StrategyContext* ctx);

  ~ConditionOrderManager();

  ConditionOrderId AddConditionOrder(const ConditionOrder& order);

  void Start(ConditionOrderId condition_order_id);

  void Stop(ConditionOrderId condition_order_id);

  void StopAll();

  std::optional<ConditionOrderExecutionDetails> QueryConditionOrder(
      ConditionOrderId condition_order_id) const;

  bool IsRunning(ConditionOrderId condition_order_id) const;

  std::vector<ConditionOrderExecutionDetails> CollectAndClearStoppedOrders();

  std::vector<ConditionOrderExecutionDetails> CollectStoppedOrders();

  std::optional<ConditionOrderExecutionDetails> CollectAndClearOrderIfStopped(
      ConditionOrderId condition_order_id);

  void ClearStoppedOrders();

  void OnDepth(const DepthData& depth);

  void OnOrder(const OrderResponse& order);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
