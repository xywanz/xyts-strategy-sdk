#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <memory>
#include <queue>

#include "nlohmann/json.hpp"
#include "xyts/utils/websocket/websocket_handler.h"

namespace xyts {

class SecureWebsocketSession : public WebsocketSession {
 public:
  SecureWebsocketSession(WebsocketHandler* handler, boost::asio::io_context& ioc,
                         boost::asio::ssl::context& ssl, std::string_view host,
                         std::string_view port, std::string_view target);

  ~SecureWebsocketSession();

  void Start();

  void Send(nlohmann::json&& msg) final;

 private:
  boost::asio::awaitable<void> WebsocketClient();

  boost::asio::awaitable<void> Writer();

  boost::asio::awaitable<void> PeriodicPong();

  WebsocketHandler* handler_;

  std::queue<nlohmann::json> snd_queue_;

  boost::asio::io_context& ioc_;
  boost::asio::steady_timer condition_;
  boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ws_;
  std::string host_;
  std::string port_;
  std::string target_;
};

}  // namespace xyts
