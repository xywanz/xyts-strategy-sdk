#pragma once

#include <filesystem>

#include "xyts/trade_api/trade_api.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

TradeApiPtr CreateTradeApi(const std::string& name, const YAML::Node& conf);

TradeApiPtr LoadTradeApiFromDll(const std::filesystem::path& path, const YAML::Node& conf);

}  // namespace xyts
