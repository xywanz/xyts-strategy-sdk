#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "xyts/core/position.h"
#include "xyts/trade_api/order_event.h"
#include "xyts/trade_api/request.h"
#include "xyu/queue/spsc/ring_buffer.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

struct TradeInfo {
  std::vector<OrderRecord> orders;
  std::vector<TradeRecord> trades;
  std::vector<Position> positions;
  std::vector<InitialYdPosition> init_yd_positions;
};

class TradeApi {
 public:
  virtual ~TradeApi() = default;

  virtual bool SendOrder(const OrderRequest& request) = 0;

  virtual bool CancelOrder(const CancellationRequest& request) = 0;

  virtual TradeInfo QueryTradeInfo() = 0;

  virtual std::vector<Contract> QueryContracts() = 0;

  virtual std::vector<Account> QueryAccounts() = 0;

  auto* GetEventRing() { return &event_ring_; }

 protected:
  template <class EventType>
  void PushEvent(const EventType& event) {
    auto* ptr = event_ring_.PrepareEnqueueBlocking();
    *ptr = event;
    event_ring_.CommitEnqueue();
  }

 private:
  using OrderEventRing = xyu::SPSCRingBuffer<OrderEvent, 1024>;
  OrderEventRing event_ring_;
};

using TradeApiPtr = std::unique_ptr<TradeApi>;

#define EXPORT_TRADE_API(type)                                            \
  extern "C" ::xyts::TradeApiPtr CreateTradeApi(const YAML::Node& conf) { \
    return std::make_unique<type>(conf);                                  \
  }

}  // namespace xyts
