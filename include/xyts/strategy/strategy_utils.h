#pragma once

#include <span>

#include "xyts/core/contract_table.h"
#include "xyts/core/market_data.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_context.h"
#include "xyu/datetime.h"

namespace xyts::strategy {

double CalculateWorstPrice(std::span<const double> prices, std::span<const Volume> volumes,
                           Volume volume);

double CalculateWorstPrice(const DepthData& depth, Volume volume, bool is_bid);

double CalculateAvgPrice(std::span<const double> prices, std::span<const Volume> volumes,
                         Volume volume);

double CalculateAvgPrice(const DepthData& depth, Volume volume, bool is_bid);

double CalculateMidPrice(const DepthData& depth);

double CalculateVolumeWeigtedPrice(const DepthData& depth);

// 不适用于法定节假日等特殊情况·
xyu::datetime::date GetTradingDay(const xyu::datetime::datetime& dt);

xyu::datetime::date GetTradingDay(std::chrono::microseconds ts);

xyu::datetime::date GetTradingDay(StrategyContext* ctx);

}  // namespace xyts::strategy
