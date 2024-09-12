#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "xyts/core/market_data.h"
#include "xyts/core/timeout_manager.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_param_manager.h"

namespace xyts::strategy {

class StrategyContext : public TimeoutManager {
 public:
  virtual ~StrategyContext() = default;

  virtual void SubscribeMarketData(const std::vector<std::string>& patterns,
                                   MarketDataType type = MarketDataType::kDepthData) = 0;

  virtual void UnsubscribeMarketData(const std::vector<std::string>& patterns,
                                     MarketDataType type = MarketDataType::kDepthData) = 0;

  virtual void SubscribePosition(const std::vector<std::string>& patterns) = 0;

  virtual void UnsubscribePosition(const std::vector<std::string>& patterns) = 0;

  virtual void SubscribeTopics(const std::vector<TopicId>& topics) = 0;

  virtual void UnsubscribeTopics(const std::vector<TopicId>& topics) = 0;

  virtual std::chrono::microseconds GetWallTime() const = 0;

  virtual void SendAlarm(const std::string& alarm_msg) = 0;

  virtual void Stop() = 0;

  virtual ClientOrderId SendOrder(ContractId contract_id, Volume volume, Direction direction,
                                  PositionEffect position_effect, OrderType type, double price,
                                  std::chrono::microseconds timeout = kDefaultOrderTimeout,
                                  uint64_t user_data = 0) = 0;

  ClientOrderId Buy(ContractId contract_id, Volume volume, OrderType type, double price,
                    std::chrono::microseconds timeout = kDefaultOrderTimeout,
                    uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kAuto, type, price,
                     timeout, user_data);
  }

  ClientOrderId BuySmart(ContractId contract_id, Volume volume, OrderType type, double price,
                         std::chrono::microseconds timeout = kDefaultOrderTimeout,
                         uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kSmart, type, price,
                     timeout, user_data);
  }

  ClientOrderId BuyLimit(ContractId contract_id, Volume volume, double price,
                         std::chrono::microseconds timeout = kDefaultOrderTimeout,
                         uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kAuto, OrderType::kLimit,
                     price, timeout, user_data);
  }

  ClientOrderId BuyLimitSmart(ContractId contract_id, Volume volume, double price,
                              std::chrono::microseconds timeout = kDefaultOrderTimeout,
                              uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kSmart,
                     OrderType::kLimit, price, timeout, user_data);
  }

  ClientOrderId BuyMarket(ContractId contract_id, Volume volume, uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kAuto,
                     OrderType::kMarket, 0, std::chrono::milliseconds{500}, user_data);
  }

  ClientOrderId BuyMarketSmart(ContractId contract_id, Volume volume, uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kSmart,
                     OrderType::kMarket, 0, std::chrono::milliseconds{500}, user_data);
  }

  ClientOrderId Sell(ContractId contract_id, Volume volume, OrderType type, double price,
                     std::chrono::microseconds timeout = kDefaultOrderTimeout,
                     uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kAuto, type, price,
                     timeout, user_data);
  }

  ClientOrderId SellSmart(ContractId contract_id, Volume volume, OrderType type, double price,
                          std::chrono::microseconds timeout = kDefaultOrderTimeout,
                          uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kSmart, type, price,
                     timeout, user_data);
  }

  ClientOrderId SellLimit(ContractId contract_id, Volume volume, double price,
                          std::chrono::microseconds timeout = kDefaultOrderTimeout,
                          uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kAuto,
                     OrderType::kLimit, price, timeout, user_data);
  }

  ClientOrderId SellLimitSmart(ContractId contract_id, Volume volume, double price,
                               std::chrono::microseconds timeout = kDefaultOrderTimeout,
                               uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kSmart,
                     OrderType::kLimit, price, timeout, user_data);
  }

  ClientOrderId SellMarket(ContractId contract_id, Volume volume, uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kAuto,
                     OrderType::kMarket, 0, std::chrono::milliseconds{500}, user_data);
  }

  ClientOrderId SellMarketSmart(ContractId contract_id, Volume volume, uint64_t user_data = 0) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kSmart,
                     OrderType::kMarket, 0, std::chrono::milliseconds{500}, user_data);
  }

  virtual void CancelOrder(ClientOrderId client_order_id) = 0;

  virtual void ResetOrderTimeout(ClientOrderId client_order_id,
                                 std::chrono::microseconds timeout) = 0;

  virtual std::vector<PositionData> GetPositions() const = 0;

  virtual PositionData GetPosition(ContractId contract_id) const = 0;

  virtual std::vector<LogicalPositionData> GetLogicalPositions() const = 0;

  virtual LogicalPositionData GetLogicalPosition(ContractId contract_id) const = 0;

  virtual std::vector<TradeRecord> GetTrades(ContractId contract_id) const = 0;

  virtual std::vector<TradeRecord> GetTrades() const = 0;

  virtual std::vector<OrderRecord> GetOrders(ContractId contract_id) const = 0;

  virtual std::vector<OrderRecord> GetOrders() const = 0;

  virtual double GetPnl() const = 0;

  virtual Account GetAccount(const std::string& account_name) const = 0;

  virtual std::vector<DepthData> GetTodayMarketData(
      const std::vector<std::string>& patterns,
      std::chrono::microseconds begin_ts = std::chrono::microseconds{0},
      std::chrono::microseconds end_ts = std::chrono::microseconds{-1UL}) const = 0;

  virtual bool PublishMessage(TopicId topic, const void* data, std::size_t size) = 0;

  virtual std::string GetStrategyName() const = 0;

  virtual StrategyParamManager* GetParamManager() = 0;

  template <class T>
  T* GetDerivedParamManager() {
    return dynamic_cast<T*>(GetParamManager());
  }

  static constexpr std::chrono::microseconds kDefaultOrderTimeout = std::chrono::days{1};
};

using EventId = StrategyContext::EventId;

}  // namespace xyts::strategy
