#pragma once

#include <memory>

#include "xyts/core/contract_table.h"
#include "xyts/core/market_data.h"
#include "xyts/strategy/state_monitor.h"

namespace xyts::strategy {

class LimitPriceChecker {
 public:
  explicit LimitPriceChecker(double critical_rate = 0.01, double recovery_rate = 0.02)
      : critical_rate_(critical_rate), recovery_rate_(recovery_rate) {}

  bool IsNearLimitPrice(const DepthData& depth);

 private:
  bool CheckOutOfLimit(ContractPtr contract, double px) const {
    const double ub = contract->upper_limit_price * (1 - critical_rate_);
    const double lb = contract->lower_limit_price * (1 + critical_rate_);
    return px >= ub || px <= lb;
  }

  bool CheckRecovery(ContractPtr contract, double px) const {
    const double ub = contract->upper_limit_price * (1 - recovery_rate_);
    const double lb = contract->lower_limit_price * (1 + recovery_rate_);
    return px <= ub && px >= lb;
  }

  std::unordered_map<ContractId, StateMonitor<double>> states_;
  double critical_rate_;
  double recovery_rate_;
};

}  // namespace xyts::strategy
