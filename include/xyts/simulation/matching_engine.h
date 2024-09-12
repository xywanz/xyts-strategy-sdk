#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "xyts/core/market_data.h"
#include "xyts/simulation/order_event_listener.h"
#include "xyts/trade_api/request.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

class MatchingEngine {
 public:
  virtual ~MatchingEngine() = default;

  virtual void AddOrder(const OrderRequest& order) = 0;

  virtual void CancelOrder(TradeId order_id, ContractId contract_id) = 0;

  virtual void MatchOnDepth(const DepthData& depth) = 0;

  void AddListener(OrderEventListener* listener) { listeners_.emplace_back(listener); }

 protected:
  void PublishTradeEvent(OrderId order_id, Volume volume, double price) {
    for (auto* listener : listeners_) {
      listener->OnTrade(order_id, volume, price);
    }
  }

  void PublishCancellationEvent(OrderId order_id, Volume cancelled_volume) {
    for (auto* listener : listeners_) {
      listener->OnCancellation(order_id, cancelled_volume);
    }
  }

  std::vector<OrderEventListener*> listeners_;
};

using MatchingEnginePtr = std::unique_ptr<MatchingEngine>;

}  // namespace xyts
