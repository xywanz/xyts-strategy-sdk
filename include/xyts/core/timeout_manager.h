#pragma once

#include <chrono>
#include <cstdint>
#include <functional>

namespace xyts {

class TimeoutManager {
 public:
  using EventId = int64_t;
  using Callback = std::function<void(EventId)>;

  virtual ~TimeoutManager() {}

  virtual EventId AddTimeout(std::chrono::microseconds microseconds, Callback&& callback) = 0;

  virtual EventId AddPeriodicCallback(std::chrono::microseconds microseconds,
                                      Callback&& callback) = 0;

  virtual void RemoveTimer(EventId id) = 0;
};

}  // namespace xyts
