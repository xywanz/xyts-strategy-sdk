#pragma once

#include "xyts/core/trade_msg.h"

namespace xyts {

class OrderEventListener {
 public:
  virtual ~OrderEventListener() {}

  virtual void OnTrade(OrderId order_id, Volume volume, double price) {}

  virtual void OnCancellation(OrderId order_id, Volume cancelled_volume) {}
};

}  // namespace xyts
