#pragma once

#include <map>
#include <mutex>
#include <set>

#include "xyts/extension/binance/stream_handler.h"
#include "xyts/utils/websocket/secure_websocket_session.h"

namespace xyts {

class BinanceWebsocketsBase : public std::enable_shared_from_this<BinanceWebsocketsBase>,
                              public WebsocketHandler {
 public:
  BinanceWebsocketsBase(BinanceStreamHandler* handler, boost::asio::io_context& ioc,
                        boost::asio::ssl::context& ssl,
                        std::string_view host = "stream.binance.com",
                        std::string_view port = "9443", std::string_view target = "/stream");

  virtual ~BinanceWebsocketsBase() = default;

  void SubscribeStream(std::string_view stream_name);

  void SubscribeStreams(const std::vector<std::string>& stream_names);

 private:
  void OnOpen(WebsocketSession* session) final;

  void OnClose(WebsocketSession* session) final;

  void OnError(WebsocketSession* session, const std::string& error) final;

  void OnMessage(WebsocketSession* session, const nlohmann::json& msg) final;

  struct SessionInfo {
    std::shared_ptr<WebsocketSession> session;
    std::vector<std::string> stream_names;
  };

  static constexpr std::size_t kMaxStreamNumPerConnection = 200;

  BinanceStreamHandler* handler_;
  boost::asio::io_context& ioc_;
  boost::asio::ssl::context& ssl_;
  std::string host_;
  std::string port_;
  std::string target_;
  std::mutex mutex_;
  std::vector<SessionInfo> sessions_;
  std::set<std::string> stream_names_;

  using RequestId = std::size_t;
  RequestId next_id_ = 1;
};

}  // namespace xyts
