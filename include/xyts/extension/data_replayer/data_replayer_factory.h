#include "xyts/extension/data_replayer/data_replayer.h"

namespace xyts {

using DataReplayerPtr = std::unique_ptr<DataReplayer>;

class DataReplayerFactory {
 public:
  static DataReplayerPtr Create(const std::string& name,
                                const std::vector<std::string>& instruments,
                                const xyu::datetime::date& date,
                                const std::vector<TimeInterval>& intervals, const YAML::Node& conf);
};

}  // namespace xyts
