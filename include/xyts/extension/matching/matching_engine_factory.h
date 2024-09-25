#pragma once

#include <filesystem>

#include "xyts/extension/matching/matching_engine.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

using MatchingEnginePtr = std::unique_ptr<MatchingEngine>;

class MatchingEngineFactory {
 public:
  static MatchingEnginePtr Create(const std::string& type, const YAML::Node& conf);

  static MatchingEnginePtr CreateFromDll(const std::filesystem::path& path, const YAML::Node& conf);

  using CreateMatchingEngineFn = MatchingEnginePtr (*)(const YAML::Node&);
  using MatchingEngineCtorMap = std::map<std::string, CreateMatchingEngineFn>;

  static MatchingEngineCtorMap& GetMatchingEngineCtorMap();
};

#define REGISTER_MATCHING_ENGINE(name, type)                                                       \
  static ::xyts::MatchingEnginePtr CreateMatchingEngine##type(const YAML::Node& conf) {            \
    return std::make_unique<type>(conf);                                                           \
  }                                                                                                \
  static const bool kIs##type##Registered [[gnu::unused]] = [] {                                   \
    ::xyts::MatchingEngineFactory::GetMatchingEngineCtorMap()[name] = &CreateMatchingEngine##type; \
    return true;                                                                                   \
  }();

// 如果是编译成动态库，则调用下面接口导出符号
#define EXPORT_MATCHING_ENGINE(type)                                                    \
  extern "C" ::xyts::MatchingEnginePtr CreateMatchingEngine(const ::YAML::Node& conf) { \
    return std::make_unique<type>(conf);                                                \
  }

}  // namespace xyts
