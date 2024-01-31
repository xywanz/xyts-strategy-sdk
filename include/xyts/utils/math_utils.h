#pragma once

#include <cstdint>

namespace xyts {

template <class RealType>
inline bool IsEqual(const RealType& lhs, const RealType& rhs, RealType epsilon = RealType(1e-6)) {
  return rhs - epsilon <= lhs && lhs <= rhs + epsilon;
}

// 向上取有效价格
// 只适用于price_tick为固定值的品种
// 最大精度到第5位小数. e.g. 0.00015 --> 0.0002，且价格有效位数应小于9
static inline double CeilPrice(double px, double price_tick) {
  const double scalar = 100000.0;

  int64_t int_price_tick = static_cast<int64_t>(price_tick * scalar);
  int64_t int_price = static_cast<int64_t>(px * scalar);
  int64_t mod = int_price % int_price_tick;
  return mod == 0 ? (int_price / scalar) : (int_price - mod + int_price_tick) / scalar;
}

// 向下取有效价格
// 只适用于price_tick为固定值的品种
// 最大精度到第5位小数. e.g. 0.00015 --> 0.0001，且价格有效位数应小于9
static inline double FloorPrice(double px, double price_tick) {
  const double scalar = 100000.0;

  int64_t int_price_tick = static_cast<int64_t>(price_tick * scalar);
  int64_t int_price = static_cast<int64_t>(px * scalar);
  int64_t mod = int_price % int_price_tick;
  return (int_price - mod) / scalar;
}

// 向上取有效数量
static inline int CeilVolume(int vol, int unit_vol) {
  auto mod = vol % unit_vol;
  return vol % unit_vol == 0 ? vol : (vol - mod + unit_vol);
}

// 向下取有效数量
static inline int FloorVolume(int vol, int unit_vol) { return vol - (vol % unit_vol); }

}  // namespace xyts
