#pragma once

#include "xyts/extension/binance/rest_api_base.h"

namespace xyts {

class BinanceFuturesRestApi : public BinanceRestApiBase {
 public:
  BinanceFuturesRestApi(boost::asio::io_context& ioc, boost::asio::ssl::context& ssl,
                        const std::string& api_key, const std::string& secret_key,
                        const std::string& host = "fapi.binance.com",
                        const std::string& port = "443");

  std::string GetListenKey();

  nlohmann::json GetExchangeInfo(const nlohmann::json& params = {});

  nlohmann::json NewOrder(const std::string& symbol, const std::string& side,
                          const std::string& type, const std::string& time_in_force,
                          const std::string& quantity, const std::string& price,
                          const std::string& new_client_order_id);

  void NewOrderAsync(const std::string& symbol, const std::string& side, const std::string& type,
                     const std::string& time_in_force, const std::string& quantity,
                     const std::string& price, const std::string& new_client_order_id,
                     Callback&& cb);

  nlohmann::json CancelOrder(const std::string& symbol, const std::string& orig_client_order_id);

  void CancelOrderAsync(const std::string& symbol, const std::string& orig_client_order_id,
                        Callback&& cb);

  nlohmann::json CancelOrders(const std::string& symbol);

  nlohmann::json QueryOrder(const std::string& symbol, const std::string& orig_client_order_id);

  nlohmann::json QueryOpenOrders(const std::string& symbol = "");

  nlohmann::json QueryAllOrders(const std::string& symbol, const std::string& start_time,
                                const std::string& end_time);

  nlohmann::json QueryAccount();

  nlohmann::json QueryBalance();
};

}  // namespace xyts
