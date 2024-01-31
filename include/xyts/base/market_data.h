#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "xyts/base/trade_msg.h"

namespace xyts {

constexpr int kMarketL1Depth = 5;
constexpr int kMarketL2Depth = 10;

constexpr int kSourceLen = 12;

constexpr int kMaxUserDefinedDataLen = 496;

#pragma pack(1)

struct MarketDataHeader {
  char source[kSourceLen];  // 行情源
  uint32_t ticker_id;
  std::chrono::microseconds local_timestamp;
  std::chrono::microseconds exchange_timestamp;
};

struct TickData : public MarketDataHeader {
  double last_price;
  uint64_t volume;
  double turnover;
  uint64_t open_interest;
  // high和low为当日最高最低价，暂时只支持股票的
  double high_price;
  double low_price;

  double ask[kMarketL2Depth];
  double bid[kMarketL2Depth];
  int ask_volume[kMarketL2Depth];
  int bid_volume[kMarketL2Depth];
};

enum class BarPeriod : uint64_t {
  M1 = 1,
  M5 = 5,
  M15 = 15,
  // H1 = 60, 还不支持
};

// K线数据
struct BarData : public MarketDataHeader {
  BarPeriod period;

  double open_price;
  double close_price;
  double low_price;
  double high_price;

  uint64_t volume;
  double turnover;
};

struct UserDefinedData {
  uint16_t channel_id;
  uint16_t data_type;
  uint16_t version;
  uint16_t size;
  char buffer[kMaxUserDefinedDataLen];
};

enum class MarketDataType : uint64_t {
  kTickData,
  kBarData,
  kUserDefinedData,
};

struct MarketData {
  MarketDataType type;

  union {
    MarketDataHeader header;

    TickData tick;
    BarData bar;
    UserDefinedData user_defined_data;
  };
};

constexpr int a = sizeof(MarketData);

#pragma pack()

inline bool IsValidBarPeriod(BarPeriod period) {
  return period == BarPeriod::M1 || period == BarPeriod::M5 || period == BarPeriod::M15;
}

static_assert(std::is_trivially_copyable_v<MarketData>);

}  // namespace xyts
