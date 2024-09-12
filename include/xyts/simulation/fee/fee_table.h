#pragma once

#include <memory>
#include <vector>

#include "xyts/core/contract_table.h"
#include "xyts/simulation/fee/fee.h"
#include "xyts/simulation/fee/fee_factory.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

class FeeTable {
 public:
  explicit FeeTable(const YAML::Node& conf = {}) : fees_(ContractTable::size()) {
    if (!conf.IsDefined() || conf.IsNull()) {
      return;
    }
    for (const auto& item : conf) {
      auto pattern = item["instr"].as<std::string>();
      auto contracts = ContractTable::GetByPattern(pattern);
      for (const auto* contract : contracts) {
        auto fee = FeeFactory::New(item);
        fees_.at(contract->contract_id) = std::move(fee);
      }
    }
  }

  double CalculateOrderSubmissionFee(const OrderRequest& order) {
    auto& comm = fees_[order.contract->contract_id];
    if (comm) {
      return comm->CalculateOrderSubmissionFee(order);
    }
    return 0;
  }

  double CalculateCancellationFee(const OrderRequest& order) {
    auto& fee = fees_[order.contract->contract_id];
    if (fee) {
      return fee->CalculateCancellationFee(order);
    }
    return 0;
  }

  double CalculateTradeFee(const OrderRequest& order, Volume volume, double price, double amount) {
    auto& fee = fees_[order.contract->contract_id];
    if (fee) {
      return fee->CalculateTradeFee(order, volume, price, amount);
    }
    return 0;
  }

 private:
  std::vector<std::unique_ptr<Fee>> fees_;
};

}  // namespace xyts
