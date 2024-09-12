#pragma once

#include <cassert>
#include <optional>

#include "xyts/core/market_data.h"

namespace xyts::strategy {

enum class DepthBehavior {
  kUnknown = 0,
  kNoChange,
  kExchangeLong,
  kExchangeShort,
  kExchange,
  kOpenLong,
  kOpenShort,
  kOpenDouble,
  kOpen,
  kCloseLong,
  kCloseShort,
  kCloseDouble,
  kClose,
};

class DepthBehaviorAnalyzer {
 public:
  DepthBehavior Analyze(const DepthData& depth);

  DepthBehavior depth_behavior() const { return behavior_; }

  Volume delta_volume() const { return delta_vol_; }

  Volume delta_position() const { return delta_pos_; }

  double last_price() const { return last_price_; }

 private:
  DepthBehavior behavior_ = DepthBehavior::kUnknown;
  Volume delta_vol_ = 0;
  Volume delta_pos_ = 0;

  Volume volume_ = 0;
  Volume open_interest_ = 0;
  double last_price_ = std::numeric_limits<double>::quiet_NaN();
  double bid_ = std::numeric_limits<double>::quiet_NaN();
  double ask_ = std::numeric_limits<double>::quiet_NaN();
};

}  // namespace xyts::strategy
