#include <filesystem>

#include "xyts/extension/data_replayer/data_replayer.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

using DataReplayerPtr = std::unique_ptr<DataReplayer>;

class DataReplayerFactory {
 public:
  static DataReplayerPtr Create(const std::string& type, const YAML::Node& conf);

  static DataReplayerPtr CreateFromDll(const std::filesystem::path& path, const YAML::Node& conf);

  using CreateDataReplayerFn = DataReplayerPtr (*)(const YAML::Node&);
  using DataReplayerCtorMap = std::map<std::string, CreateDataReplayerFn>;

  static DataReplayerCtorMap& GetDataReplayerCtorMap();
};

#define REGISTER_DATA_REPLAYER(name, cls)                                                   \
  static ::xyts::DataReplayerPtr CreateDataReplayer##cls(const YAML::Node& conf) {          \
    return std::make_unique<cls>(conf);                                                     \
  }                                                                                         \
  static const bool kIsDataReplayer##cls##Registered [[gnu::unused]] = [] {                 \
    ::xyts::DataReplayerFactory::GetDataReplayerCtorMap()[name] = &CreateDataReplayer##cls; \
    return true;                                                                            \
  }();

#define EXPORT_DATA_REPLAYER(cls)                                                   \
  extern "C" ::xyts::DataReplayerPtr CreateDataReplayer(const ::YAML::Node& conf) { \
    return std::make_unique<cls>(conf);                                             \
  }

}  // namespace xyts
