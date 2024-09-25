#pragma once

#include <string>

#include "nlohmann/json.hpp"
#include "xyts/utils/websocket/websocket_session.h"

namespace xyts {

class WebsocketHandler {
 public:
  virtual ~WebsocketHandler() = default;

  virtual void OnOpen(WebsocketSession* session) = 0;

  virtual void OnClose(WebsocketSession* session) = 0;

  virtual void OnError(WebsocketSession* session, const std::string& error) = 0;

  virtual void OnMessage(WebsocketSession* session, const nlohmann::json& msg) = 0;
};

}  // namespace xyts
