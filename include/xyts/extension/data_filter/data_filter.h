#pragma once

#include <memory>

#include "xyts/core/market_data.h"

namespace xyts {

class DataFilter {
 public:
  virtual ~DataFilter() = default;

  virtual bool Accept(const DepthData& depth) { return true; }

  virtual bool Accept(const BarData& bar) { return true; }

  virtual bool Accept(const TopicMessage& msg) { return true; }
};

}  // namespace xyts
