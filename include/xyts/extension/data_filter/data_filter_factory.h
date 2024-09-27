#pragma once

#include <filesystem>
#include <map>
#include <memory>

#include "xyts/extension/data_filter/data_filter.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

using DataFilterPtr = std::unique_ptr<DataFilter>;

class DataFilterFactory {
 public:
  static DataFilterPtr Create(const std::string& name, const YAML::Node& conf);

  static DataFilterPtr CreateFromDll(const std::filesystem::path& path, const YAML::Node& conf);

  using CreateDataFilterFn = DataFilterPtr (*)(const YAML::Node&);
  using DataFilterCtorMap = std::map<std::string, CreateDataFilterFn>;

  static DataFilterCtorMap& GetDataFilterCtorMap();
};

// 内部编写的filter可以直接注册
#define REGISTER_DATA_FILTER(name, cls)                                               \
  static ::xyts::DataFilterPtr CreateDataFilter##cls(const YAML::Node& conf) {        \
    return std::make_unique<cls>(conf);                                               \
  }                                                                                   \
  static const bool kIs##cls##Registered [[gnu::unused]] = [] {                       \
    ::xyts::DataFilterFactory::GetDataFilterCtorMap()[name] = &CreateDataFilter##cls; \
    return true;                                                                      \
  }();

// 如果是编译成动态库，则调用下面接口导出符号
#define EXPORT_DATA_FILTER(cls)                                                 \
  extern "C" ::xyts::DataFilterPtr CreateDataFilter(const ::YAML::Node& conf) { \
    return std::make_unique<cls>(conf, spi);                                    \
  }

}  // namespace xyts
