#pragma once

#include <memory>

#include "xyts/core/market_data.h"

namespace xyts {

class MarketDataFilter {
 public:
  virtual ~MarketDataFilter() = default;

  virtual bool Accept(const DepthData& depth) { return true; }

  virtual bool Accept(const BarData& bar) { return true; }

  virtual bool Accept(const TopicMessage& msg) { return true; }
};

}  // namespace xyts
