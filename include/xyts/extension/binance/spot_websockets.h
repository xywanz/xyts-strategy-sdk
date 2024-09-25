#pragma once

#include "xyts/extension/binance/websockets_base.h"

namespace xyts {

class BinanceSpotWebsockets final : public BinanceWebsocketsBase {
 public:
  BinanceSpotWebsockets(BinanceStreamHandler* handler, boost::asio::io_context& ioc,
                        boost::asio::ssl::context& ssl,
                        std::string_view host = "stream.binance.com",
                        std::string_view port = "9443", std::string_view target = "/stream");

  void SubscribeAggTrade(std::string_view symbol);

  void SubscribeTrade(std::string_view symbol);

  void SubscribeKline(std::string_view symbol, std::string_view interval);

  void SubscribeMiniTicker(std::string_view symbol);

  void SubscribeAllMiniTickers();

  void SubscribeTicker(std::string_view symbol);

  void SubscribeAllTickers();

  void SubscribeBookTicker(std::string_view symbol);

  void SubscribeAvgPrice(std::string_view symbol);

  void SubscribePartialDepth(std::string_view symbol, int levels,
                             std::chrono::milliseconds update_speed);

  void SubscribeDiffDepth(std::string_view symbol, std::chrono::milliseconds update_speed);

  void SubscribeTickerWindow(std::string_view symbol, std::string_view window_size);

  void SubscribeAllTickerWindows(std::string_view window_size);

  void SubscribeUserData(std::string_view listen_key);
};

}  // namespace xyts
