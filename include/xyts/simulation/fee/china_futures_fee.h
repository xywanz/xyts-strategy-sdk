#pragma once

#include "xyts/simulation/fee/fee.h"

namespace xyts {

class ChinaFuturesFee final : public Fee {
 public:
  ChinaFuturesFee(double fee_rate, double fixed_fee, double close_td_multiplier, double order_fee,
                  double cancellation_fee)
      : fee_rate_(fee_rate),
        fixed_fee_(fixed_fee),
        close_td_multiplier_(close_td_multiplier),
        order_fee_(order_fee),
        cancellation_fee_(cancellation_fee) {}

  double CalculateOrderSubmissionFee(const OrderRequest& order) final { return order_fee_; }

  double CalculateCancellationFee(const OrderRequest& order) final { return cancellation_fee_; }

  double CalculateTradeFee(const OrderRequest& order, Volume volume, double price,
                           double amount) final {
    double fee = fee_rate_ * amount + fixed_fee_ * volume;
    if (order.position_effect == PositionEffect::kCloseToday) {
      fee *= close_td_multiplier_;
    }
    return fee;
  }

 private:
  double fee_rate_;
  double fixed_fee_;
  double close_td_multiplier_;
  double order_fee_;
  double cancellation_fee_;
};

}  // namespace xyts
