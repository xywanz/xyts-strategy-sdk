#pragma once

#include "xyts/extension/fee/fee.h"

namespace xyts {

class FixedPerVolumeFee final : public Fee {
 public:
  explicit FixedPerVolumeFee(double fee_per_volume) : fee_per_volume_(fee_per_volume) {}

  double CalculateTradeFee(const OrderRequest& order, Volume volume, double price,
                           double amount) final {
    return volume * fee_per_volume_;
  }

 private:
  double fee_per_volume_ = 0;
};

}  // namespace xyts
