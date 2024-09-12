#pragma once

#include <memory>

#include "xyts/simulation/fee/china_futures_fee.h"
#include "xyts/simulation/fee/fee.h"
#include "xyts/simulation/fee/fixed_per_volume_fee.h"
#include "xyts/simulation/fee/percentage_fee.h"
#include "xyts/simulation/fee/zero_fee.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

class FeeFactory {
 public:
  static std::unique_ptr<Fee> New(const YAML::Node& conf) {
    auto type = conf["type"].as<std::string>();
    if (type == "none") {
      return std::make_unique<ZeroFee>();
    } else if (type == "fixed_per_volume") {
      auto fee_per_volume = conf["fee_per_volume"].as<double>();
      return std::make_unique<FixedPerVolumeFee>(fee_per_volume);
    } else if (type == "percentage") {
      auto rate = conf["rate"].as<double>();
      return std::make_unique<PercentageFee>(rate);
    } else if (type == "china_futures") {
      double fee_rate = conf["fee_rate"].as<double>(0);
      double fixed_fee = conf["fixed_fee"].as<double>(0);
      double close_td_multiplier = conf["close_td_multiplier"].as<double>(1);
      double order_fee = conf["order_fee"].as<double>(0);
      double cancellation_fee = conf["cancellation_fee"].as<double>(0);
      return std::make_unique<ChinaFuturesFee>(fee_rate, fixed_fee, close_td_multiplier, order_fee,
                                               cancellation_fee);
    }
    return nullptr;
  }
};

}  // namespace xyts
