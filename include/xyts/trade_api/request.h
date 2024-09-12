#pragma once

#include <cstdint>

#include "xyts/core/contract_table.h"
#include "xyts/core/trade_msg.h"

namespace xyts {

struct OrderRequest {
  ContractPtr contract;
  OrderId order_id;
  OrderType type;
  Direction direction;
  PositionEffect position_effect;
  Volume volume;
  double price;
};

struct CancellationRequest {
  ContractPtr contract;
  CancellationId cancellation_id;
  OrderId order_id;
  OrderSysId order_sys_id;
};

}  // namespace xyts
