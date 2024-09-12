#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "xyu/time.h"

namespace xyu {

class ThreadPool {
 public:
  explicit ThreadPool(std::size_t thread_num) {
    for (std::size_t i = 0; i < thread_num; ++i) {
      threads_.emplace_back([this](std::stop_token stop_token) { Worker(stop_token); });
    }
  }

  ~ThreadPool() {
    for (auto& thread : threads_) {
      thread.request_stop();
    }
    cv_.notify_all();
  }

  template <class F, class... Args>
  void AddTask(F&& f, Args&&... args) {
    auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    Publish(std::move(func));
  }

  template <class Duration, class F, class... Args>
  void AddDelayTask(const Duration& delay, F&& f, Args&&... args) {
    auto now = GetMonotonicTimeNs();
    auto expiration = now + delay;
    Publish(std::bind(std::forward<F>(f), std::forward<Args>(args)...), expiration);
  }

  template <class Duration, class F, class... Args>
  void AddRepeatTask(const Duration& interval, F&& f, Args&&... args) {
    auto now = GetMonotonicTimeNs();
    auto itv = std::chrono::duration_cast<nanoseconds>(interval);
    auto expiration = now + itv;
    Publish(std::bind(std::forward<F>(f), std::forward<Args>(args)...), expiration, itv);
  }

 private:
  using nanoseconds = std::chrono::nanoseconds;

  struct Task {
    std::function<void()> func;
    nanoseconds interval;
  };

  void Publish(std::function<void()> func, nanoseconds expiration = nanoseconds{0},
               nanoseconds interval = nanoseconds{-1}) {
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.emplace(expiration, Task{std::move(func), interval});
    cv_.notify_one();
  }

  void Worker(std::stop_token& stop_token) {
    while (!stop_token.stop_requested()) {
      std::unique_lock<std::mutex> lock(mutex_);
      if (tasks_.empty()) {
        cv_.wait(lock,
                 [this, &stop_token] { return !tasks_.empty() || stop_token.stop_requested(); });
        continue;
      }
      auto next_task_iter = tasks_.begin();
      auto next_expiration = next_task_iter->first;
      auto now = GetMonotonicTimeNs();
      if (next_expiration > now) {
        cv_.wait_for(lock, next_expiration - now,
                     [&stop_token] { return stop_token.stop_requested(); });
        continue;
      }
      auto next_task = std::move(next_task_iter)->second;
      tasks_.erase(next_task_iter);
      lock.unlock();
      next_task.func();
      if (next_task.interval > nanoseconds{0}) {
        AddRepeatTask(next_task.interval, std::move(next_task.func));
      }
    }
  }

  std::multimap<nanoseconds, Task> tasks_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::vector<std::jthread> threads_;
};

}  // namespace xyu
