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
                                  std::chrono::microseconds timeout = kNeverTimeout) = 0;

  ClientOrderId Buy(ContractId contract_id, Volume volume, OrderType type, double price,
                    std::chrono::microseconds timeout = kNeverTimeout) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kAuto, type, price);
  }

  ClientOrderId BuySmart(ContractId contract_id, Volume volume, OrderType type, double price,
                         std::chrono::microseconds timeout = kNeverTimeout) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kSmart, type, price);
  }

  ClientOrderId BuyLimit(ContractId contract_id, Volume volume, double price,
                         std::chrono::microseconds timeout = kNeverTimeout) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kAuto, OrderType::kLimit,
                     price, timeout);
  }

  ClientOrderId BuyLimitSmart(ContractId contract_id, Volume volume, double price,
                              std::chrono::microseconds timeout = kNeverTimeout) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kSmart,
                     OrderType::kLimit, price, timeout);
  }

  ClientOrderId BuyMarket(ContractId contract_id, Volume volume) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kAuto,
                     OrderType::kMarket, 0, std::chrono::milliseconds{500});
  }

  ClientOrderId BuyMarketSmart(ContractId contract_id, Volume volume) {
    return SendOrder(contract_id, volume, Direction::kBuy, PositionEffect::kSmart,
                     OrderType::kMarket, 0, std::chrono::milliseconds{500});
  }

  ClientOrderId Sell(ContractId contract_id, Volume volume, OrderType type, double price,
                     std::chrono::microseconds timeout = kNeverTimeout) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kAuto, type, price);
  }

  ClientOrderId SellSmart(ContractId contract_id, Volume volume, OrderType type, double price,
                          std::chrono::microseconds timeout = kNeverTimeout) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kSmart, type, price);
  }

  ClientOrderId SellLimit(ContractId contract_id, Volume volume, double price,
                          std::chrono::microseconds timeout = kNeverTimeout) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kAuto,
                     OrderType::kLimit, price, timeout);
  }

  ClientOrderId SellLimitSmart(ContractId contract_id, Volume volume, double price,
                               std::chrono::microseconds timeout = kNeverTimeout) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kSmart,
                     OrderType::kLimit, price, timeout);
  }

  ClientOrderId SellMarket(ContractId contract_id, Volume volume) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kAuto,
                     OrderType::kMarket, 0, std::chrono::milliseconds{500});
  }

  ClientOrderId SellMarketSmart(ContractId contract_id, Volume volume) {
    return SendOrder(contract_id, volume, Direction::kSell, PositionEffect::kSmart,
                     OrderType::kMarket, 0, std::chrono::milliseconds{500});
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
      std::chrono::microseconds end_ts = std::chrono::microseconds::max()) const = 0;

  virtual bool PublishMessage(TopicId topic, const void* data, std::size_t size) = 0;

  virtual std::string GetStrategyName() const = 0;

  virtual StrategyParamManager* GetParamManager() = 0;

  template <class T>
  T* GetDerivedParamManager() {
    return dynamic_cast<T*>(GetParamManager());
  }

  static constexpr std::chrono::microseconds kNeverTimeout = std::chrono::microseconds::max();
};

using EventId = StrategyContext::EventId;

}  // namespace xyts::strategy
