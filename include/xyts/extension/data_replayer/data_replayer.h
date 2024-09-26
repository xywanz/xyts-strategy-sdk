#pragma once

#include <functional>

#include "xyts/core/market_data.h"

namespace xyts {

class DataReplayer {
 public:
  using DepthDataHandler = std::function<void(const DepthData&)>;

  virtual ~DataReplayer() = default;

  virtual void ReplayAll(const DepthDataHandler& handler) = 0;
};

}  // namespace xyts
