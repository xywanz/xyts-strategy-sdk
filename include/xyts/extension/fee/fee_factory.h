#pragma once

#include <memory>

#include "xyts/extension/fee/fee.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

using FeePtr = std::unique_ptr<Fee>;

class FeeFactory {
 public:
  static FeePtr Create(const YAML::Node& conf);
};

}  // namespace xyts
