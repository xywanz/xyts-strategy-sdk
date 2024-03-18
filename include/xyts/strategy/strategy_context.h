#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "xyts/core/timeout_manager.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_param_manager.h"

namespace xyts::strategy {

class StrategyContext : public TimeoutManager {
 public:
  virtual ~StrategyContext() = default;

  virtual void Subscribe(const std::vector<std::string>& patterns) = 0;

  virtual void SubscribeTopics(const std::vector<Topic>& topics) = 0;

  virtual void RegisterPublisher() = 0;

  virtual std::chrono::microseconds GetWallTime() const = 0;

  virtual void Alarm(const std::string& alarm_msg) = 0;

  virtual void Stop() = 0;

  virtual ClientOrderId SendOrder(ContractId contract_id, Volume volume, Direction direction,
                                  Offset offset, OrderType type, double price,
                                  std::chrono::microseconds timeout = kDefaultOrderTimeout,
                                  uint64_t user_data = 0) = 0;

  ClientOrderId Buy(ContractId contract_id, Volume volume, OrderType type, double price,
                    std::chrono::microseconds timeout = kDefaultOrderTimeout,
                    uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kBuy, Offset::kAuto, type, price, timeout,
                     user_data);
  }

  ClientOrderId Sell(ContractId contract_id, Volume volume, OrderType type, double price,
                     std::chrono::microseconds timeout = kDefaultOrderTimeout,
                     uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kSell, Offset::kAuto, type, price, timeout,
                     user_data);
  }

  virtual void CancelOrder(ClientOrderId client_order_id) = 0;

  virtual void ResetOrderTimeout(ClientOrderId client_order_id,
                                 std::chrono::microseconds timeout) = 0;

  virtual std::vector<PositionData> GetPositions() const = 0;

  virtual PositionData GetPosition(ContractId contract_id) const = 0;

  virtual std::vector<LogicalPositionData> GetLogicalPositions() const = 0;

  virtual LogicalPositionData GetLogicalPosition(ContractId contract_id) const = 0;

  virtual std::vector<Fill> GetFills(ContractId contract_id) const = 0;

  virtual std::vector<Fill> GetFills() const = 0;

  virtual Account GetAccount(const std::string& account_name) const = 0;

  virtual bool PublishMessage(uint32_t channel_id, uint32_t data_type, const void* data,
                              std::size_t size) = 0;

  virtual std::string GetStrategyName() const = 0;

  virtual StrategyParamManager* GetParamManager() = 0;

  static constexpr std::chrono::microseconds kDefaultOrderTimeout = std::chrono::days{1};
};

using EventId = StrategyContext::EventId;

}  // namespace xyts::strategy
