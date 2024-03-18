#pragma once

#include <filesystem>

#include "xyts/data_feed/data_feed_api.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

DataFeedApiPtr CreateDataFeedApi(const std::string& name, const YAML::Node& conf, DataFeedSpi* spi);

DataFeedApiPtr LoadDataFeedApiFromDll(const std::filesystem::path& path, const YAML::Node& conf,
                                      DataFeedSpi* spi);

}  // namespace xyts
