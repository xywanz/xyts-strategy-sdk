#pragma once

#include <cmath>
#include <cstdint>

#include "xyts/core/trade_msg.h"

namespace xyts {

static constexpr double kEpsilon = 1e-9;

template <class RealType>
inline constexpr bool IsEqual(RealType lhs, RealType rhs, RealType epsilon = RealType(1e-9)) {
  return std::abs(lhs - rhs) < epsilon;
}

inline constexpr bool IsZero(double v, double epsilon = 1e-9) { return IsEqual(v, 0.0, epsilon); }

// 向上取有效价格
// 只适用于price_tick为固定值的品种
// e.g. 0.00015 --> 0.0002
inline constexpr double CeilPrice(double px, double price_tick) {
  return std::ceil(px / price_tick) * price_tick;
}

// 向下取有效价格
// 只适用于price_tick为固定值的品种
// e.g. 0.00015 --> 0.0001
inline constexpr double FloorPrice(double px, double price_tick) {
  return std::floor(px / price_tick) * price_tick;
}

inline constexpr double RoundPrice(double px, double price_tick) {
  return std::round(px / price_tick) * price_tick;
}

// 向上取有效数量
inline constexpr Volume CeilVolume(Volume vol, Volume unit_vol) {
  auto mod = vol % unit_vol;
  return vol % unit_vol == 0 ? vol : (vol - mod + unit_vol);
}

// 向下取有效数量
inline constexpr Volume FloorVolume(Volume vol, Volume unit_vol) { return vol - (vol % unit_vol); }

inline constexpr double NaNPrice() { return std::numeric_limits<double>::quiet_NaN(); }

}  // namespace xyts
