#pragma once

#include <atomic>
#include <cassert>

#include "xyu/misc.h"

namespace xyu {

template <typename T, uint64_t capacity>
class SPSCRingBuffer {
  static_assert(capacity != 0 && (capacity & (capacity - 1)) == 0, "capacity must be power of 2");

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

  void CommitDequeue() {
    consumer_pos_.store(consumer_pos_.load(std::memory_order_relaxed) + 1,
                        std::memory_order_release);
  }

 private:
  static constexpr uint64_t kIndexMask_ = capacity - 1;

  alignas(2 * kCacheLineSize) char padding_;
  alignas(2 * kCacheLineSize) std::atomic<uint64_t> producer_pos_{0};
  alignas(2 * kCacheLineSize) std::atomic<uint64_t> consumer_pos_{0};
  alignas(2 * kCacheLineSize) T* buffer_ = nullptr;
};

}  // namespace xyu
