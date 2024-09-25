#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/system/error_code.hpp>
#include <cstdint>
#include <functional>
#include <span>
#include <string>

#include "nlohmann/json.hpp"

namespace xyts {

class HttpsRequests : public std::enable_shared_from_this<HttpsRequests> {
 public:
  using RequestType = boost::beast::http::request<boost::beast::http::string_body>;
  using HttpResponse = boost::beast::http::response<boost::beast::http::string_body>;
  using Callback = std::function<void(HttpResponse&&, const std::string&)>;

  HttpsRequests(boost::asio::io_context& ioc, boost::asio::ssl::context& ssl,
                const std::string& host, const std::string& port);

  void SetHeader(nlohmann::json&& header);

  HttpResponse Request(boost::beast::http::verb method, const std::string& target,
                       const std::string& data);

  void RequestAsync(boost::beast::http::verb method, const std::string& target,
                    const std::string& data, Callback cb);

 private:
  boost::asio::awaitable<void> RequestCoro(boost::beast::http::verb method, std::string target,
                                           std::string data, Callback cb);

  RequestType BuildRequest(boost::beast::http::verb method, const std::string& target,
                           const std::string& data);

  static constexpr int kHttpVersion = 11;

  boost::asio::io_context& ioc_;
  boost::asio::ssl::context& ssl_;
  std::string host_;
  std::string port_;
  nlohmann::json header_;
};

}  // namespace xyts
