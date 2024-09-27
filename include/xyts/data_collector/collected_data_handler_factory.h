#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "xyts/data_collector/collected_data_handler.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

using CollectedDataHandlerPtr = std::unique_ptr<CollectedDataHandler>;

class CollectedDataHandlerFactory {
 public:
  static CollectedDataHandlerPtr Create(const std::string& type, const YAML::Node& conf);

  static CollectedDataHandlerPtr CreateFromDll(const std::filesystem::path& path,
                                               const YAML::Node& conf);

  using CreateCollectedDataHandlerFn = CollectedDataHandlerPtr (*)(const YAML::Node&);
  using CollectedDataHandlerCtorMap = std::map<std::string, CreateCollectedDataHandlerFn>;

  static CollectedDataHandlerCtorMap& GetCollectedDataHandlerCtorMap();
};

#define REGISTER_COLLECTED_DATA_HANDLER(name, cls)                                                 \
  static ::xyts::CollectedDataHandlerPtr CreateCollectedDataHandler##cls(const YAML::Node& conf) { \
    return std::make_unique<cls>(conf);                                                            \
  }                                                                                                \
  static const bool kIs##cls##Registered [[gnu::unused]] = [] {                                    \
    ::xyts::CollectedDataHandlerFactory::GetCollectedDataHandlerCtorMap()[name] =                  \
        &CreateCollectedDataHandler##cls;                                                          \
    return true;                                                                                   \
  }();

// 如果是编译成动态库，则调用下面接口导出符号
#define EXPORT_COLLECTED_DATA_HANDLER(cls)                               \
  extern "C" ::xyts::CollectedDataHandlerPtr CreateCollectedDataHandler( \
      const ::YAML::Node& conf) {                                        \
    return std::make_unique<cls>(conf);                                  \
  }

}  // namespace xyts
