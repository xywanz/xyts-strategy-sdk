#pragma once

#include <chrono>
#include <memory>

#include "nlohmann/json.hpp"
#include "xyts/core/market_data.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_context.h"
#include "xyts/strategy/strategy_param_manager.h"

namespace xyts::strategy {

class Strategy {
 public:
  virtual ~Strategy() = default;

  /**
   * @brief 收到订阅的深度行情数据
   *
   * @param depth DepthData
   */
  virtual void OnDepth(const DepthData& depth) {}

  /**
   * @brief 订单状态更新推送
   * 订单状态有任何变动都会推送该消息。
   * @param order
   */
  virtual void OnOrder(const OrderResponse& order) {}

  /**
   * @brief 收到更新配置请求
   * 通常来自于外部进行参数更新，可重写该函数用于需要监听参数更新请求的场景。
   * @param updated_params
   */
  virtual void OnParamUpdate(const nlohmann::json& updated_params) {}

  /**
   * @brief 收到控制命令
   * 通过网络连接对策略进行控制
   * @param command
   */
  virtual void OnCommand(const nlohmann::json& command) {}

  /**
   * @brief 收到自定义数据
   */
  virtual void OnMessage(const TopicMessage& msg) {}
};

#define EXPORT_STRATEGY(type)                                                                    \
  extern "C" std::unique_ptr<::xyts::strategy::Strategy> CreateStrategy(                         \
      ::xyts::strategy::StrategyContext* ctx) {                                                  \
    return std::make_unique<type>(ctx);                                                          \
  }                                                                                              \
  extern "C" std::unique_ptr<::xyts::strategy::StrategyParamManager> CreateStrategyParamManager( \
      const char* param_path) {                                                                  \
    return std::make_unique<type##ParamManager>(param_path);                                     \
  }

}  // namespace xyts::strategy
