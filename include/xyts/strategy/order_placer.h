#pragma once

#include <chrono>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "xyts/core/contract_table.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_context.h"
#include "xyts/utils/math_utils.h"

namespace xyts::strategy {

using OrderPlaceId = uint64_t;

// TODO
// INCOMPLETE
class OrderPlacer {
 public:
  explicit OrderPlacer(StrategyContext* ctx, const std::string& name = "");

  ~OrderPlacer();

  void PlaceOrder(ContractId contract_id, Direction direction, Volume volume, double price,
                  OrderPlaceId id,
                  std::chrono::microseconds timeout = kDefaultTimeout) noexcept(false);

  bool AnyOrderInTheMarket(ContractId contract_id, OrderPlaceId id) const;

  bool AnyOrderInTheMarket(ContractId contract_id, Direction direction, OrderPlaceId id) const;

  bool AnyOrderInTheMarket(ContractId contract_id) const;

  void OnOrder(const OrderResponse& order);

  static constexpr std::chrono::microseconds kDefaultTimeout = std::chrono::days{1};

 private:
  struct Order {
    ContractId contract_id = 0;
    uint64_t inner_order_id = 0;
    ClientOrderId client_order_id = 0;
    OrderId order_id = 0;
    Direction direction;
    // 下单时指定的offset，可以是auto
    PositionEffect position_effect;
    // 实际的offset，经过交易系统转换后的
    PositionEffect real_position_effect;
    double price;
    Volume original_volume = 0;
    Volume accum_trade_volume = 0;
    std::chrono::microseconds creation_time{0};
    std::chrono::microseconds timeout{0};
    bool active = false;
  };

  struct Task {
    double price = NaNPrice();
    Volume total_volume = 0;
    Volume accum_trade_volume = 0;
    std::list<Order> orders;
    std::unordered_set<ClientOrderId> orders_waiting_for_cancellation;
  };

  struct ContractTaskInfo {
    std::unordered_map<OrderPlaceId, Task> tasks;
  };

  using CoidTaskMap = std::unordered_map<ClientOrderId, Task*>;

  StrategyContext* ctx_;
  std::string name_;

  std::vector<ContractTaskInfo> contract_buy_task_infos_;
  std::vector<ContractTaskInfo> contract_sell_task_infos_;
  CoidTaskMap coid_task_map_;
};

}  // namespace xyts::strategy
