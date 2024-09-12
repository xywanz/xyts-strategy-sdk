#pragma once

#include <functional>

namespace xyts::strategy {

template <class T>
class StateMonitor {
 public:
  using EnterCondFunc = std::function<bool(const T&)>;
  using ExitCondFunc = EnterCondFunc;

  explicit StateMonitor(EnterCondFunc&& enter_cond, ExitCondFunc&& exit_cond = nullptr,
                        bool initial_state = false)
      : active_(initial_state),
        enter_cond_(std::move(enter_cond)),
        exit_cond_(std::move(exit_cond)) {}

  bool Update(const T& v) {
    if (active_) {
      if (exit_cond_) {
        active_ = !exit_cond_(v);
      } else if (enter_cond_) {
        active_ = enter_cond_(v);
      }
    } else {
      if (enter_cond_) {
        active_ = enter_cond_(v);
      }
    }
    return active_;
  }

  bool IsActive() const { return active_; }

 private:
  bool active_;
  EnterCondFunc enter_cond_;
  ExitCondFunc exit_cond_;
};

}  // namespace xyts::strategy
