#pragma once

#include <map>

#include "xyts/core/contract_table.h"

namespace xyts::strategy {

class PnlCalculator {
 public:
  explicit PnlCalculator(const std::string& name = "", bool print_pnl_on_exit = true)
      : name_(name), print_pnl_on_exit_(print_pnl_on_exit) {}

  ~PnlCalculator();

  double CalculatePnl() const;

  Volume GetTotalFill() const;

  void UpdatePnlOnTrade(ContractPtr contract, Direction direction, Volume volume, double price);

  void UpdatePnlOnPriceChange(ContractPtr contract, double last_price);

 private:
  struct FillRecord {
    double contract_unit = 1;
    Volume long_fill = 0;
    Volume short_fill = 0;
    double long_amount = 0;
    double short_amount = 0;
    double last_price = std::numeric_limits<double>::quiet_NaN();
  };

  FillRecord* GetOrCreateFillRecord(ContractPtr contract) {
    if (auto it = fill_records_.find(contract->instr); it != fill_records_.end()) {
      return &it->second;
    }
    auto [it, _] = fill_records_.emplace(contract->instr, FillRecord{contract->contract_unit});
    return &it->second;
  }

  std::map<std::string, FillRecord> fill_records_;
  std::string name_;
  bool print_pnl_on_exit_;
};

}  // namespace xyts::strategy
