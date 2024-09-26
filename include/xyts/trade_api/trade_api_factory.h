#pragma once

#include <filesystem>

#include "xyts/trade_api/trade_api.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

class TradeApiFactory {
 public:
  static TradeApiPtr Create(const std::string& name, const YAML::Node& conf);

  static TradeApiPtr CreateFromDll(const std::filesystem::path& path, const YAML::Node& conf);
};

}  // namespace xyts
