#pragma once

#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <string>

#include "xyts/core/market_data.h"
#include "xyu/datetime.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

struct TimeInterval {
  xyu::datetime::time begin_time;
  xyu::datetime::time end_time;
};

class DataReplayer {
 public:
  using DepthDataHandler = std::function<void(const DepthData&)>;

  virtual ~DataReplayer() = default;

  virtual void ReplayAll(const DepthDataHandler& handler) = 0;
};

}  // namespace xyts
