#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "xydata/depth.h"
#include "xydata/trading_calendar.h"
#include "xyu/datetime.h"
#include "xyu/filesystem.h"

namespace xydata {

class ContractManager {
 public:
  explicit ContractManager(const std::filesystem::path& xydata_dir)
      : xydata_dir_(xyu::fs::ExpandUser(xydata_dir)),
        contract_dir_(xydata_dir_ / "main_contract"),
        calendar_(xydata_dir_ / "holiday"),
        depth_loader_(xydata_dir_) {}

  std::string GetMainContract(const std::string& symbol, const xyu::datetime::date& date);

  std::string GetMainContract(const std::string& symbol, const std::string& date) {
    return GetMainContract(symbol, xyu::datetime::date::fromisoformat(date));
  }

 private:
  std::string LoadMainContract(const std::string& symbol, const xyu::datetime::date& date);
  std::string LoadOrCalcMainContract(const std::string& symbol, const xyu::datetime::date& date,
                                     bool dump = false);
  std::string CalcMainContract(const std::string& symbol, const xyu::datetime::date& date,
                               bool dump = false);
  void DumpMainContract(const std::string& symbol, const std::string& instr,
                        const xyu::datetime::date& date);
  bool IsLastTradingDay(const std::string& symbol, const std::string& instr,
                        const xyu::datetime::date& date);

  std::filesystem::path xydata_dir_;
  std::filesystem::path contract_dir_;
  TradingCalendar calendar_;
  DepthLoader depth_loader_;
};

}  // namespace xydata
