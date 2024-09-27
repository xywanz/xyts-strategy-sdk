#pragma once

#include <memory>
#include <string>

#include "xyts/data_collector/collected_data_type.h"
#include "xyu/utility.h"

namespace xyts {

class DataCollector : public xyu::NonCopyableNonMoveable {
 public:
  ~DataCollector();

  static void Collect(CollectedDataType type, const std::string& data);

  static bool IsRunning();

 private:
  DataCollector();

  void Collect_(CollectedDataType type, const std::string& data);

  bool IsRunning_() const;

  static DataCollector& Instance();

  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts
