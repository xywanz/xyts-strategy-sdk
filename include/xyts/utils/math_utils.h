#pragma once

#include <cmath>
#include <cstdint>

namespace xyts {

static constexpr double kEpsilon = 1e-9;

template <class RealType>
inline bool IsEqual(const RealType& lhs, const RealType& rhs, RealType epsilon = RealType(1e-6)) {
  return rhs - epsilon <= lhs && lhs <= rhs + epsilon;
}

// 向上取有效价格
// 只适用于price_tick为固定值的品种
// e.g. 0.00015 --> 0.0002
static inline double CeilPrice(double px, double price_tick) {
  return std::ceil(px / price_tick) * price_tick;
}

// 向下取有效价格
// 只适用于price_tick为固定值的品种
// e.g. 0.00015 --> 0.0001
static inline double FloorPrice(double px, double price_tick) {
  return std::floor(px / price_tick) * price_tick;
}

// 向上取有效数量
static inline int64_t CeilVolume(int64_t vol, int64_t unit_vol) {
  auto mod = vol % unit_vol;
  return vol % unit_vol == 0 ? vol : (vol - mod + unit_vol);
}

// 向下取有效数量
static inline int64_t FloorVolume(int64_t vol, int64_t unit_vol) { return vol - (vol % unit_vol); }

}  // namespace xyts
