#pragma once

#include <filesystem>

#include "xyts/data_feed/data_feed_api.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

class DataFeedApiFactory {
 public:
  static DataFeedApiPtr Create(const std::string& name, const YAML::Node& conf, DataFeedSpi* spi);

  static DataFeedApiPtr CreateFromDll(const std::filesystem::path& path, const YAML::Node& conf,
                                      DataFeedSpi* spi);
};

}  // namespace xyts
