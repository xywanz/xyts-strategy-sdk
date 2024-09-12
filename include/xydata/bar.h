#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "xydata/contract.h"
#include "xydata/depth.h"
#include "xydata/sessions.h"
#include "xydata/trading_calendar.h"
#include "xyu/datetime.h"

namespace xydata {

struct Bar {
  std::string instr;
  std::chrono::microseconds timestamp;

  double open_price;
  double close_price;
  double low_price;
  double high_price;

  Volume volume;
  double turnover;
};

struct BarInterval {
  xyu::datetime::time begin;
  xyu::datetime::time end;
  xyu::datetime::time bar_time;
};

std::vector<BarInterval> SplitToBarIntervals(const std::vector<Session>& sessions,
                                             std::chrono::seconds period);

class BarLoader {
 public:
  explicit BarLoader(const std::filesystem::path& xydata_dir)
      : bar_dir_(xydata_dir / "bar"),
        calendar_(xydata_dir / "holiday"),
        depth_loader_(xydata_dir),
        contract_mgr_(xydata_dir) {}

  std::vector<Bar> LoadBars(const std::string& instr, std::chrono::seconds period,
                            const xyu::datetime::date& begin_date,
                            const xyu::datetime::date& end_date, bool force_gen = false);

  std::vector<Bar> LoadBars(const std::string& instr, std::chrono::seconds period,
                            const std::string& begin_date, const std::string& end_date,
                            bool force_gen = false) {
    return LoadBars(instr, period, xyu::datetime::date::fromisoformat(begin_date),
                    xyu::datetime::date::fromisoformat(end_date), force_gen);
  }

  std::vector<Bar> LoadMainBars(const std::string& symbol, std::chrono::seconds period,
                                const xyu::datetime::date& begin_date,
                                const xyu::datetime::date& end_date, bool force_gen = false);

  std::vector<Bar> LoadMainBars(const std::string& symbol, std::chrono::seconds period,
                                const std::string& begin_date, const std::string& end_date,
                                bool force_gen = false) {
    return LoadMainBars(symbol, period, xyu::datetime::date::fromisoformat(begin_date),
                        xyu::datetime::date::fromisoformat(end_date), force_gen);
  }

 private:
  void LoadBars(std::vector<Bar>* out, const std::string& instr, std::chrono::seconds period,
                const xyu::datetime::date& begin_date, const xyu::datetime::date& end_date,
                bool force_gen = false);

  void GenerateAndDumpBars(const std::filesystem::path& path, const std::string& instr,
                           std::chrono::seconds period, const xyu::datetime::date& date);

  std::vector<Bar> GenerateBarsFromDepths(const std::string& instr, std::chrono::seconds period,
                                          const xyu::datetime::date& date);

  std::filesystem::path bar_dir_;
  TradingCalendar calendar_;
  DepthLoader depth_loader_;
  ContractManager contract_mgr_;
};

}  // namespace xydata
