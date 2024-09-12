#pragma once

#include <chrono>
#include <cstdint>
#include <variant>

#include "xyts/core/error_code.h"
#include "xyts/core/trade_msg.h"

namespace xyts {

struct OrderAcceptedEvent {
  OrderId order_id;
  OrderSysId order_sys_id;
};

struct OrderRejectedEvent {
  OrderId order_id;
  ErrorCode error_code = ErrorCode::kRejected;
};

struct OrderCanceledEvent {
  OrderId order_id;
  Volume canceled_volume;
};

struct CancellationRejectedEvent {
  OrderId order_id;
};

struct TradeEvent {
  std::chrono::microseconds local_timestamp;
  std::chrono::microseconds exchange_timestamp;
  TradeId trade_sys_id;
  OrderId order_id;
  OrderSysId order_sys_id;
  Volume volume;
  double price;
};

using OrderEvent = std::variant<OrderAcceptedEvent, OrderRejectedEvent, TradeEvent,
                                OrderCanceledEvent, CancellationRejectedEvent>;

}  // namespace xyts
