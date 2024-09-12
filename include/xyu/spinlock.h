#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include "xyu/misc.h"

namespace xyu {

class WaitOptions {
 public:
  struct Defaults {
    static constexpr std::chrono::nanoseconds kSpinMax = std::chrono::microseconds(2);
  };

  explicit WaitOptions() = default;

  constexpr WaitOptions(std::chrono::nanoseconds spin_max) : spin_max_(spin_max) {}

  constexpr std::chrono::nanoseconds spin_max() const { return spin_max_; }

 private:
  std::chrono::nanoseconds spin_max_ = Defaults::kSpinMax;
};

enum class SpinResult {
  kSuccess,
  kTimeout,
  kAdvance,
};

template <typename Clock, typename Duration, typename F>
SpinResult SpinPauseUntil(const std::chrono::time_point<Clock, Duration>& deadline,
                          const WaitOptions& opt, F f) {
  if (opt.spin_max() <= opt.spin_max().zero()) {
    return SpinResult::kAdvance;
  }

  auto tbegin = Clock::now();
  while (true) {
    if (f()) {
      return SpinResult::kSuccess;
    }

    auto const tnow = Clock::now();
    if (tnow >= deadline) {
      return SpinResult::kTimeout;
    }

    tbegin = std::min(tbegin, tnow);
    if (tnow >= tbegin + opt.spin_max()) {
      return SpinResult::kAdvance;
    }

    cpu_pause();
  }
}

template <typename Clock, typename Duration, typename F>
SpinResult SpinYieldUntil(const std::chrono::time_point<Clock, Duration>& deadline, F f) {
  while (true) {
    if (f()) {
      return SpinResult::kSuccess;
    }

    auto const max = std::chrono::time_point<Clock, Duration>::max();
    if (deadline != max && Clock::now() >= deadline) {
      return SpinResult::kTimeout;
    }

    std::this_thread::yield();
  }
}

class SpinLock {
 public:
  SpinLock() = default;
  SpinLock(const SpinLock&) = delete;
  SpinLock(SpinLock&&) = delete;

  SpinLock& operator=(const SpinLock&) = delete;
  SpinLock& operator=(SpinLock&&) = delete;

  void lock() {
    uint32_t ticket = ticket_.fetch_add(1);
    while (owner_.load(std::memory_order_acquire) != ticket) {
      continue;
    }
  }

  void unlock() {
    uint32_t cur = owner_.load(std::memory_order_relaxed);
    owner_.store(cur + 1, std::memory_order_release);
  }

  bool TryLock() {
    uint32_t expected = owner_.load(std::memory_order_relaxed);
    return ticket_.compare_exchange_strong(expected, expected + 1, std::memory_order_acquire);
  }

  template <typename Clock, typename Duration>
  bool TryLockUntil(const std::chrono::time_point<Clock, Duration>& deadline,
                    const WaitOptions& opt = WaitOptions{}) {
    switch (SpinPauseUntil(deadline, opt, [this] { return TryLock(); })) {
      case SpinResult::kSuccess:
        return true;
      case SpinResult::kTimeout:
        return false;
      case SpinResult::kAdvance:
        break;
    }

    while (true) {
      auto res = SpinYieldUntil(deadline, [this] { return TryLock(); });
      if (res == SpinResult::kSuccess) {
        return true;
      } else if (res == SpinResult::kTimeout) {
        return false;
      }
    }
    return true;
  }

 private:
  std::atomic<uint32_t> ticket_{0};
  std::atomic<uint32_t> owner_{0};
};

}  // namespace xyu
