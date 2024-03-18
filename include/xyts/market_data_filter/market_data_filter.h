#pragma once

#include <memory>

#include "xyts/core/market_data.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

class MarketDataFilter {
 public:
  virtual ~MarketDataFilter() = default;

  virtual bool Accept(const TickData& tick) = 0;
};

using MarketDataFilterPtr = std::unique_ptr<MarketDataFilter>;

}  // namespace xyts
