#pragma once

#include "xyts/extension/fee/fee.h"

namespace xyts {

class InvalidFeeRate final : public std::invalid_argument {
 public:
  explicit InvalidFeeRate(double rate)
      : std::invalid_argument("Fee rate must be within [0, 1) but got " + std::to_string(rate)) {}
};

class PercentageFee final : public Fee {
 public:
  explicit PercentageFee(double rate) : rate_(rate) {
    if (rate_ >= 1 || rate < 0) {
      throw InvalidFeeRate(rate);
    }
  }

  double CalculateTradeFee(const OrderRequest& order, Volume volume, double price,
                           double amount) final {
    return amount * rate_;
  }

 private:
  double rate_ = 0.0;
};

}  // namespace xyts
