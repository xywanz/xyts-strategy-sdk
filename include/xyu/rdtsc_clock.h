// require constant_tsc
#pragma once

#include <x86intrin.h>

#include <cstdint>

namespace xyu {

class RdtscClock {
 public:
  static RdtscClock& Instance() {
    static RdtscClock rdtsc_clock;
    return rdtsc_clock;
  }

  double nano_per_tick() const { return nano_per_tick_; }

 private:
  RdtscClock();

 private:
  double nano_per_tick_ = 0.0;
};

inline uint64_t rdtsc() noexcept { return __rdtsc(); }
inline int64_t tsc_to_nano(uint64_t tsc) noexcept {
  return tsc * RdtscClock::Instance().nano_per_tick();
}

}  // namespace xyu
