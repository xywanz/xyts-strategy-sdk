#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "xyts/base/timeout_manager.h"
#include "xyts/base/trade_msg.h"
#include "xyts/strategy/strategy_param_manager.h"

namespace xyts::strategy {

class StrategyContext : public TimeoutManager {
 public:
  virtual ~StrategyContext() {}

  virtual bool Subscribe(const std::vector<std::string>& patterns) = 0;

  virtual bool SubscribeChannels(const std::vector<ChannelSubscriptionInfo>& channels) = 0;

  virtual bool RegisterMsgSender() = 0;

  virtual std::chrono::microseconds GetWallTime() const = 0;

  virtual void Alarm(const std::string& alarm_msg) = 0;

  virtual void Stop() = 0;

  virtual ClientOrderId SendOrder(uint32_t ticker_id, int volume, Direction direction,
                                  Offset offset, OrderType type, double price,
                                  std::chrono::microseconds timeout = kDefaultOrderTimeout,
                                  uint64_t user_data = 0) = 0;

  ClientOrderId Buy(uint32_t ticker_id, int volume, OrderType type, double price,
                    std::chrono::microseconds timeout = kDefaultOrderTimeout,
                    uint64_t user_data = 0) {
    return SendOrder(ticker_id, volume, Direction::kBuy, Offset::kAuto, type, price, timeout,
                     user_data);
  }

  ClientOrderId Sell(uint32_t ticker_id, int volume, OrderType type, double price,
                     std::chrono::microseconds timeout = kDefaultOrderTimeout,
                     uint64_t user_data = 0) {
    return SendOrder(ticker_id, volume, Direction::kSell, Offset::kAuto, type, price, timeout,
                     user_data);
  }

  virtual void CancelOrder(ClientOrderId client_order_id) = 0;

  virtual void ResetOrderTimeout(ClientOrderId client_order_id,
                                 std::chrono::microseconds timeout) = 0;

  virtual std::vector<PositionData> GetPosition() const = 0;

  virtual PositionData GetPosition(const std::string& instr) const = 0;

  virtual std::vector<LogicalPositionData> GetLogicalPosition() const = 0;

  virtual LogicalPositionData GetLogicalPosition(const std::string& instr) const = 0;

  virtual std::vector<Fill> GetFills(const std::string& instr) const = 0;

  virtual std::vector<Fill> GetFills() const = 0;

  virtual Account GetAccount(const std::string& account_name) const = 0;

  virtual bool SendMessage(uint16_t channel_id, uint16_t data_type, const void* data,
                           std::size_t size, uint16_t version) = 0;

  virtual std::string GetStrategyName() const = 0;

  virtual StrategyParamManager* GetParamManager() = 0;

  static constexpr std::chrono::microseconds kDefaultOrderTimeout =
      std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::days{1});
};

using EventId = StrategyContext::EventId;

}  // namespace xyts::strategy
