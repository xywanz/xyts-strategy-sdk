#pragma once

#include <chrono>
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

  EventId AddTimeout(std::chrono::milliseconds milliseconds, Callback&& callback) {
    return AddTimeout(std::chrono::duration_cast<std::chrono::microseconds>(milliseconds),
                      std::move(callback));
  }

  EventId AddTimeout(std::chrono::seconds seconds, Callback&& callback) {
    return AddTimeout(std::chrono::duration_cast<std::chrono::microseconds>(seconds),
                      std::move(callback));
  }

  EventId AddPeriodicCallback(std::chrono::milliseconds milliseconds, Callback&& callback) {
    return AddPeriodicCallback(std::chrono::duration_cast<std::chrono::microseconds>(milliseconds),
                               std::move(callback));
  }

  EventId AddPeriodicCallback(std::chrono::seconds seconds, Callback&& callback) {
    return AddPeriodicCallback(std::chrono::duration_cast<std::chrono::microseconds>(seconds),
                               std::move(callback));
  }
};

}  // namespace xyts
