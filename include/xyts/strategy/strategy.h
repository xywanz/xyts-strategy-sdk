#pragma once

#include <chrono>

#include "nlohmann/json.hpp"
#include "xyts/base/market_data.h"
#include "xyts/base/trade_msg.h"
#include "xyts/strategy/strategy_context.h"
#include "xyts/strategy/strategy_param_manager.h"

namespace xyts::strategy {

class Strategy {
 public:
  virtual ~Strategy() = default;

  /**
   * @brief 收到订阅的行情数据
   *
   * @param tick TickData
   */
  virtual void OnTick(const TickData& tick) {}

  /**
   * @brief 订单状态更新推送
   * 订单状态有任何变动都会推送该消息。
   * OnOrder, OnTrade以及OnPosition的顺序为：
   * OnPhysicalPosition --> OnLogicalPosition --> OnOrder --> OnTrade
   * @param order
   */
  virtual void OnOrder(const OrderResponse& order) {}

  /**
   * @brief 成交推送
   *
   * @param trade
   */
  virtual void OnTrade(const OrderResponse& trade) {}

  /**
   * @brief 物理仓位发生变动
   *
   * @param position 最新的仓位
   */
  virtual void OnPosition(const PositionData& position) {}

  /**
   * @brief 策略逻辑仓位发生变动
   *
   * @param position 最新的仓位
   */
  virtual void OnPosition(const LogicalPositionData& position) {}

  /**
   * @brief 收到更新配置请求
   * 通常来自于外部进行参数更新，可重写该函数用于需要监听参数更新请求的场景。
   * @param updated_params
   */
  virtual void OnUpdatingParam(const nlohmann::json& updated_params) {}

  /**
   * @brief 收到自定义数据
   *
   * @param channel
   * @param data_type
   * @param buffer
   * @param version
   */
  virtual void OnMessage(uint16_t channel_id, uint16_t data_type, const char* buffer,
                         uint16_t version) {}
};

#define EXPORT_STRATEGY(type)                                                                     \
  extern "C" ::xyts::strategy::Strategy* CreateStrategy(::xyts::strategy::StrategyContext* ctx) { \
    return new type(ctx);                                                                         \
  }                                                                                               \
  extern "C" ::xyts::strategy::StrategyParamManager* CreateStrategyParamManager(                  \
      const char* param_path) {                                                                   \
    return new type##ParamManager(param_path);                                                    \
  }

}  // namespace xyts::strategy
