#pragma once

#include <vector>

#include "nlohmann/json.hpp"

namespace xyts {

class BinanceStreamHandler {
 public:
  virtual ~BinanceStreamHandler() = default;

  virtual void OnOpen(const std::vector<std::string>& stream_names) = 0;

  virtual void OnClose(const std::vector<std::string>& stream_names, const std::string& error) = 0;

  virtual void OnMessage(const nlohmann::json& msg) = 0;
};

}  // namespace xyts
