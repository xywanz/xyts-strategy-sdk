#pragma once

#include <string>
#include <vector>

#include "xyts/data_feed/data_feed_spi.h"
#include "xyu/utility.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

enum class DataFeedStatus : int {
  kNotInitialized,
  kConnecting,
  kConntected,
  kDisconnected,
  kLogonSucceeded,
  kLogonFailed,
  kReady,
};

class DataFeedApi : public xyu::NonCopyableNonMoveable {
 public:
  virtual ~DataFeedApi() = default;

  virtual DataFeedStatus GetStatus() const = 0;

  virtual bool Subscribe(const std::vector<std::string>& patterns) = 0;

  virtual bool Unsubscribe(const std::vector<std::string>& patterns) = 0;
};

using DataFeedApiPtr = std::unique_ptr<DataFeedApi>;

#define EXPORT_DATA_FEED_API(type)                                                \
  extern "C" ::xyts::DataFeedApiPtr CreateDataFeedApi(const ::YAML::Node& conf,   \
                                                      ::xyts::DataFeedSpi* spi) { \
    return std::make_unique<type>(conf, spi);                                     \
  }

}  // namespace xyts
