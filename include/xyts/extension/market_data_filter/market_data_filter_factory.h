#pragma once

#include <filesystem>
#include <map>
#include <memory>

#include "xyts/extension/market_data_filter/market_data_filter.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

using MarketDataFilterPtr = std::unique_ptr<MarketDataFilter>;

class MarketDataFilterFactory {
 public:
  static MarketDataFilterPtr Create(const std::string& name, const YAML::Node& conf);

  static MarketDataFilterPtr CreateFromDll(const std::filesystem::path& path,
                                           const YAML::Node& conf);

  using CreateMarketDataFilterFn = MarketDataFilterPtr (*)(const YAML::Node&);
  using MarketDataFilterCtorMap = std::map<std::string, CreateMarketDataFilterFn>;

  static MarketDataFilterCtorMap& GetMarketDataFilterCtorMap();
};

// 内部编写的filter可以直接注册
#define REGISTER_MARKET_DATA_FILTER(name, cls)                                             \
  static ::xyts::MarketDataFilterPtr CreateMarketDataFilter##cls(const YAML::Node& conf) { \
    return std::make_unique<cls>(conf);                                                    \
  }                                                                                        \
  static const bool kIs##cls##Registered [[gnu::unused]] = [] {                            \
    ::xyts::MarketDataFilterFactory::GetMarketDataFilterCtorMap()[name] =                  \
        &CreateMarketDataFilter##cls;                                                      \
    return true;                                                                           \
  }();

// 如果是编译成动态库，则调用下面接口导出符号
#define EXPORT_MARKET_DATA_FILTER(cls)                                                      \
  extern "C" ::xyts::MarketDataFilterPtr CreateMarketDataFilter(const ::YAML::Node& conf) { \
    return std::make_unique<cls>(conf, spi);                                                \
  }

}  // namespace xyts
