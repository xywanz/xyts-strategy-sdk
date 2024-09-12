#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "xyts/core/trade_msg.h"

namespace xyts {

constexpr int kMarketL1Depth = 5;
constexpr int kMarketL2Depth = 10;

constexpr int kSourceLen = 12;

constexpr int kMaxTopicMessageLen = 488;

struct MarketDataHeader {
  char source[kSourceLen];  // 行情源
  ContractId contract_id;
  std::chrono::microseconds local_timestamp;
  std::chrono::microseconds exchange_timestamp;
};

struct DepthData : public MarketDataHeader {
  double last_price;
  Volume volume;
  double turnover;
  Volume open_interest;

  std::array<double, kMarketL2Depth> ask;
  std::array<double, kMarketL2Depth> bid;
  std::array<Volume, kMarketL2Depth> ask_volume;
  std::array<Volume, kMarketL2Depth> bid_volume;
};

// K线数据
struct BarData : public MarketDataHeader {
  std::chrono::seconds period;

  double open_price;
  double close_price;
  double low_price;
  double high_price;

  Volume volume;
  double turnover;
};

struct TopicMessage {
  TopicId topic;
  std::size_t size;
  char buffer[kMaxTopicMessageLen];
};

enum class MarketDataType : uint64_t {
  kDepthData,
  kBarData,
  kTopicMessage,
};

struct MarketData {
  MarketDataType type;

  union {
    MarketDataHeader header;
    DepthData depth_data;
    BarData bar_data;

    TopicMessage topic_message;
  };
};

static_assert(std::is_trivially_copyable_v<MarketData>);
static_assert(sizeof(MarketData) % 64 == 0);

}  // namespace xyts
