#pragma once

#include <memory>

#include "xyts/core/market_data.h"
#include "xyu/utility.h"

namespace xyts {

class MarketDataWriter : public xyu::NonCopyableNonMoveable {
 public:
  virtual ~MarketDataWriter() = default;

  virtual void WriteTo(DepthData* depth) const noexcept {}

  virtual void WriteTo(TopicMessage* msg) const noexcept {}

  void operator()(DepthData* depth) const { WriteTo(depth); }
  void operator()(TopicMessage* msg) const { WriteTo(msg); }
};

class DataFeedSpi {
 public:
  virtual ~DataFeedSpi() = default;

  virtual void OnOpen() {}

  virtual void OnClose() {}

  virtual void OnDepth(const MarketDataWriter& writer) {}

  virtual void OnMessage(const MarketDataWriter& writer) {}
};

}  // namespace xyts
