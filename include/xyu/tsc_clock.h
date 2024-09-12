// require constant_tsc
#pragma once

#include <x86intrin.h>

#include <cstdint>

#include "xyu/utility.h"

namespace xyu {

class TscClock : public NonCopyableNonMoveable {
 public:
  static TscClock& Instance() {
    static TscClock tsc_clock;
    return tsc_clock;
  }

  static uint64_t ReadTsc() noexcept { return __rdtsc(); }

  static int64_t GetNanoFromTsc(uint64_t tsc) noexcept {
    return tsc * TscClock::Instance().nano_per_tick();
  }

  double nano_per_tick() const noexcept { return nano_per_tick_; }

 private:
  TscClock();

  double nano_per_tick_ = 0.0;
};

}  // namespace xyu
