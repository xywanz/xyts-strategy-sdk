#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/system/error_code.hpp>
#include <cstdint>
#include <functional>
#include <span>

#include "nlohmann/json.hpp"
#include "xyts/utils/http/https_requests.h"

namespace xyts {

class BinanceRestApiBase {
 public:
  using Response = HttpsRequests::HttpResponse;
  using Callback = HttpsRequests::Callback;

  BinanceRestApiBase(boost::asio::io_context& ioc, boost::asio::ssl::context& ssl,
                     const std::string& api_key, const std::string& secret_key,
                     const std::string& host, const std::string& port);

  virtual ~BinanceRestApiBase() = default;

 protected:
  Response Request(boost::beast::http::verb method, const std::string& target,
                   const nlohmann::json& params, bool auth);

  void RequestAsync(boost::beast::http::verb method, const std::string& target,
                    const nlohmann::json& params, bool auth, Callback&& cb);

 private:
  std::tuple<std::string, std::string> ProcessTargetAndData(boost::beast::http::verb method,
                                                            const std::string& target,
                                                            const nlohmann::json& params,
                                                            bool auth);

  static std::string EncodeParams(const nlohmann::json& params);

  static std::string HmacSha256(const std::string& key, const std::string& data);

  std::shared_ptr<HttpsRequests> https_requests_;
  std::string api_key_;
  std::string secret_key_;
};

}  // namespace xyts
