#pragma once

#include <chrono>
#include <cstdint>

#include "xyts/core/contract_table.h"
#include "xyts/core/trade_msg.h"

namespace xyts::binance::futures {

constexpr inline uint64_t kAggTradeChannel = 1000000UL << 32;
constexpr inline uint64_t kBookTickerChannel = 1000001UL << 32;
constexpr inline uint64_t kKlineChannel = 1000002UL << 32;

struct AggTrade {
  ContractId contract_id;
  bool is_buyer_maker;
  std::chrono::microseconds event_time;
  std::chrono::microseconds trade_time;
  int64_t agg_trade_id;
  double price;
  Volume volume;
  int64_t first_trade_id;
  int64_t last_trade_id;
};

struct BookTicker {
  ContractId contract_id;
  int64_t update_id;
  std::chrono::microseconds event_time;
  std::chrono::microseconds transaction_time;
  double best_bid_price;
  Volume best_bid_volume;
  double best_ask_price;
  Volume best_ask_volume;
};

struct Kline {
  ContractId contract_id;
  int interval;
  char interval_unit;
  bool is_kline_closed;
  std::chrono::microseconds event_time;
  std::chrono::microseconds kline_start_time;
  std::chrono::microseconds kline_end_time;
  int64_t first_trade_id;
  int64_t last_trade_id;
  double open_price;
  double close_price;
  double high_price;
  double low_price;
  double base_asset_volume;
  int64_t num_trades;
  double quota_asset_volume;
  double taker_buy_base_asset_volume;
  double taker_buy_quote_asset_volume;
};

}  // namespace xyts::binance::futures
