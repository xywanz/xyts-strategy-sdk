#pragma once

#include <string>

#include "xyts/data_collector/collected_data_type.h"

namespace xyts {

class CollectedDataHandler {
 public:
  virtual ~CollectedDataHandler() = default;

  virtual void Handle(CollectedDataType type, const std::string& data) = 0;
};

}  // namespace xyts
