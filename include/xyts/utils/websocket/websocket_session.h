#pragma once

#include <memory>

#include "nlohmann/json.hpp"

namespace xyts {

class WebsocketSession : public std::enable_shared_from_this<WebsocketSession> {
 public:
  virtual ~WebsocketSession() = default;

  virtual void Send(nlohmann::json&& msg) = 0;
};

}  // namespace xyts
