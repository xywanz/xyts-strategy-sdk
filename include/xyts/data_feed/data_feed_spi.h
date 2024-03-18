#pragma once

#include <memory>

#include "xyts/core/market_data.h"
#include "xyu/utility.h"

namespace xyts {

class MarketDataWriter : public xyu::NonCopyableNonMoveable {
 public:
  virtual ~MarketDataWriter() = default;

  virtual void WriteTo(TickData* tick) const noexcept {}

  virtual void WriteTo(TopicMessage* msg) const noexcept {}

  void operator()(TickData* tick) const { WriteTo(tick); }
  void operator()(TopicMessage* msg) const { WriteTo(msg); }
};

class DataFeedSpi {
 public:
  virtual ~DataFeedSpi() = default;

  virtual void OnOpen() {}

  virtual void OnClose() {}

  virtual void OnTick(const MarketDataWriter& writer) {}

  virtual void OnMessage(const MarketDataWriter& writer) {}
};

}  // namespace xyts
