#pragma once

#include <atomic>
#include <cassert>

#include "xyu/misc.h"

namespace xyu {

template <typename T, uint64_t capacity>
class SPSCRingBuffer {
  static_assert(capacity != 0 && (capacity & (capacity - 1)) == 0, "Capacity must be power of 2");

 public:
  SPSCRingBuffer() : buffer_(new T[capacity]{}) {}

  ~SPSCRingBuffer() { delete[] buffer_; }

  T* PrepareEnqueue() {
    uint64_t producer_pos = producer_pos_.load(std::memory_order_relaxed);
    if (producer_pos - consumer_pos_.load(std::memory_order_acquire) == capacity) {
      return nullptr;
    }
    return &buffer_[producer_pos & kIndexMask_];
  }

  T* PrepareEnqueueBlocking() {
    for (;;) {
      if (auto p = PrepareEnqueue(); p) {
        return p;
      }
    }
  }

  void CommitEnqueue() {
    producer_pos_.store(producer_pos_.load(std::memory_order_relaxed) + 1,
                        std::memory_order_release);
  }

  T* PrepareDequeue() {
    uint64_t consumer_pos = consumer_pos_.load(std::memory_order_relaxed);
    if (consumer_pos == producer_pos_.load(std::memory_order_acquire)) {
      return nullptr;
    }
    return &buffer_[consumer_pos & kIndexMask_];
  }

  const T* PrepareDequeueBlocking() {
    for (;;) {
      if (auto p = PrepareDequeue(); p) {
        return p;
      }
    }
  }

  void CommitDequeue() {
    consumer_pos_.store(consumer_pos_.load(std::memory_order_relaxed) + 1,
                        std::memory_order_release);
  }

  // 移除队列头的n个元素
  void Remove(std::size_t num = 1) {
    auto producer_pos = producer_pos_.load(std::memory_order_acquire);
    auto consumer_pos = consumer_pos_.load(std::memory_order_relaxed);
    auto cons_new_pos = consumer_pos + num;
    if (consumer_pos >= producer_pos) {
      consumer_pos = producer_pos;
    }
    consumer_pos_.store(cons_new_pos, std::memory_order_release);
  }

  void Clear() { Remove(size()); }

  uint64_t size() const {
    return producer_pos_.load(std::memory_order_acquire) -
           consumer_pos_.load(std::memory_order_acquire);
  }

  uint64_t free_space() const { return capacity - size(); }

  bool empty() const { return size() == 0; }

  bool full() const { return free_space() == 0; }

 private:
  static constexpr uint64_t kIndexMask_ = capacity - 1;

  alignas(kCacheLineSize) char padding_;
  alignas(kCacheLineSize) std::atomic<uint64_t> producer_pos_{0};
  alignas(kCacheLineSize) std::atomic<uint64_t> consumer_pos_{0};
  alignas(kCacheLineSize) T* buffer_ = nullptr;
};

}  // namespace xyu
