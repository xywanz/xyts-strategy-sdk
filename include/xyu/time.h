#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>

#include "xyu/datetime.h"

namespace xyu {

constexpr std::chrono::microseconds kUsPerUs = std::chrono::microseconds{1};
constexpr std::chrono::microseconds kUsPerMs = std::chrono::milliseconds{1};
constexpr std::chrono::microseconds kUsPerSecond = std::chrono::seconds{1};
constexpr std::chrono::microseconds kUsPerMinute = std::chrono::minutes{1};
constexpr std::chrono::microseconds kUsPerHour = std::chrono::hours{1};
constexpr std::chrono::microseconds kUsPerDay = std::chrono::days{1};
constexpr std::chrono::microseconds kUsPerWeek = std::chrono::weeks{1};
constexpr std::chrono::seconds kSecondsPerDay = std::chrono::days{1};

inline std::chrono::seconds GetMonotonicTimeSec() {
  return std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::steady_clock::now().time_since_epoch());
}

inline std::chrono::milliseconds GetMonotonicTimeMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now().time_since_epoch());
}

inline std::chrono::microseconds GetMonotonicTimeUs() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::steady_clock::now().time_since_epoch());
}

inline std::chrono::nanoseconds GetMonotonicTimeNs() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now().time_since_epoch());
}

inline std::chrono::seconds GetRealTimeSec() {
  return std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch());
}

inline std::chrono::milliseconds GetRealTimeMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch());
}

inline std::chrono::microseconds GetRealTimeUs() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now().time_since_epoch());
}

inline std::chrono::nanoseconds GetRealTimeNs() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::system_clock::now().time_since_epoch());
}

// 某日00:00:00时的微秒时间戳
inline std::chrono::microseconds DateBeginTimestamp(const xyu::datetime::date& date) {
  return xyu::datetime::datetime::combine(date, xyu::datetime::time()).timestamp();
}

inline std::chrono::microseconds DateBeginTimestamp(int year, int month, int day) {
  return xyu::datetime::datetime(year, month, day).timestamp();
}

inline std::chrono::microseconds DateBeginTimestamp(const std::string& isoformat_str) {
  return DateBeginTimestamp(xyu::datetime::date::fromisoformat(isoformat_str));
}

// 当天00:00:00时的微秒时间戳
inline std::chrono::microseconds TodayBeginTimestamp() {
  return DateBeginTimestamp(xyu::datetime::date::today());
}

}  // namespace xyu
