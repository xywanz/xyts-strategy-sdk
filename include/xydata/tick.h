#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <set>
#include <string>
#include <vector>

#include "xydata/trading_calendar.h"
#include "xyts/core/market_data.h"
#include "xyts/core/trade_msg.h"
#include "xyu/datetime.h"

namespace xydata {

using Volume = xyts::Volume;

struct Tick {
  std::string instr;
  std::chrono::microseconds local_timestamp;
  std::chrono::microseconds exchange_timestamp;
  double last_price;
  Volume volume;
  double turnover;
  Volume open_interest;
  double ask[xyts::kMarketL2Depth];
  double bid[xyts::kMarketL2Depth];
  Volume ask_volume[xyts::kMarketL2Depth];
  Volume bid_volume[xyts::kMarketL2Depth];
};

class TickLoader {
 public:
  explicit TickLoader(const std::filesystem::path& xydata_dir)
      : tick_dir_(xydata_dir / "tick"),
        xydata_dir_(xydata_dir),
        calendar_(xydata_dir / "holiday") {}

  std::vector<Tick> LoadTicks(const std::string& instr, const xyu::datetime::date& begin_date,
                              const xyu::datetime::date& end_date,
                              bool nothrow_if_not_exists = false);

  std::vector<Tick> LoadTicks(const std::string& instr, const std::string& begin_date,
                              const std::string& end_date, bool nothrow_if_not_exists = false) {
    return LoadTicks(instr, xyu::datetime::date::fromisoformat(begin_date),
                     xyu::datetime::date::fromisoformat(end_date), nothrow_if_not_exists);
  }

  std::vector<Tick> LoadMainTicks(const std::string& symbol, const xyu::datetime::date& begin_date,
                                  const xyu::datetime::date& end_date);

  std::vector<Tick> LoadMainTicks(const std::string& symbol, const std::string& begin_date,
                                  const std::string& end_date) {
    return LoadMainTicks(symbol, xyu::datetime::date::fromisoformat(begin_date),
                         xyu::datetime::date::fromisoformat(end_date));
  }

  std::set<std::string> GetAllInstruments(const xyu::datetime::date& date);

  std::set<std::string> GetAllInstruments(const std::string& date) {
    return GetAllInstruments(xyu::datetime::date::fromisoformat(date));
  }

  std::set<std::string> GetInstrumentsBySymbol(const std::string& symbol,
                                               const xyu::datetime::date& date);

  std::set<std::string> GetInstrumentsBySymbol(const std::string& symbol, const std::string& date) {
    return GetInstrumentsBySymbol(symbol, xyu::datetime::date::fromisoformat(date));
  }

 private:
  void LoadTicks(std::vector<Tick>* out, const std::string& instr,
                 const xyu::datetime::date& begin_date, const xyu::datetime::date& end_date,
                 bool nothrow_if_not_exists);

  std::filesystem::path tick_dir_;
  std::filesystem::path xydata_dir_;
  TradingCalendar calendar_;
};

}  // namespace xydata
