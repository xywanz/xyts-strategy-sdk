#pragma once

#include "xyts/trade_api/request.h"

namespace xyts {

class Fee {
 public:
  virtual ~Fee() = default;

  virtual double CalculateOrderSubmissionFee(const OrderRequest& order) { return 0; }

  virtual double CalculateCancellationFee(const OrderRequest& order) { return 0; }

  virtual double CalculateTradeFee(const OrderRequest& order, Volume volume, double price,
                                   double amount) {
    return 0;
  }
};

}  // namespace xyts
