#pragma once

#include "xyts/extension/binance/websockets_base.h"

namespace xyts {

class BinanceFuturesWebsockets final : public BinanceWebsocketsBase {
 public:
  BinanceFuturesWebsockets(BinanceStreamHandler* handler, boost::asio::io_context& ioc,
                           boost::asio::ssl::context& ssl,
                           std::string_view host = "fstream.binance.com",
                           std::string_view port = "443", std::string_view target = "/stream");

  void SubscribeAggTrade(std::string_view symbol);

  void SubscribeMarkPrice(std::string_view symbol, std::chrono::milliseconds update_speed);

  void SubscribeAllMarkPrices(std::chrono::milliseconds update_speed);

  void SubscribeKline(std::string_view symbol, std::string_view interval);

  void SubscribeContinuousKline(std::string_view symbol, std::string_view interval);

  void SubscribeMiniTicker(std::string_view symbol);

  void SubscribeAllMiniTickers();

  void SubscribeTicker(std::string_view symbol);

  void SubscribeAllTickers();

  void SubscribeBookTicker(std::string_view symbol);

  void SubscribeForceOrder(std::string_view symbol);

  void SubscribeAllForceOrder();

  void SubscribePartialDepth(std::string_view symbol, int levels,
                             std::chrono::milliseconds update_speed);

  void SubscribeDiffDepth(std::string_view symbol, std::chrono::milliseconds update_speed);

  void SubscribeUserData(std::string_view listen_key);
};

}  // namespace xyts
