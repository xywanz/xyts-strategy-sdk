#pragma once

#include <atomic>
#include <cassert>

#include "xyu/misc.h"

namespace xyu {

template <typename T, uint64_t capacity>
class MPMCRingBuffer {
  static_assert(capacity != 0 && (capacity & (capacity - 1)) == 0, "capacity must be power of 2");

  struct Producer {
    alignas(kCacheLineSize * 2) char padding0_;
    alignas(kCacheLineSize * 2) std::atomic<uint64_t> head{0};
    alignas(kCacheLineSize * 2) std::atomic<uint64_t> tail{0};
  };

  struct Consumer {
    alignas(kCacheLineSize * 2) std::atomic<uint64_t> head{0};
    alignas(kCacheLineSize * 2) std::atomic<uint64_t> tail{0};
  };

  static_assert(sizeof(Producer) == 6 * kCacheLineSize);
  static_assert(sizeof(Consumer) == 4 * kCacheLineSize);

 public:
  MPMCRingBuffer() : buffer_(new T[capacity]{}) {}
  ~MPMCRingBuffer() { delete[] buffer_; }

  std::pair<T*, uint64_t> PrepareEnqueue() {
    bool success;
    uint64_t cons_tail;
    uint64_t prod_head;
    uint64_t next_prod_head;

    do {
      prod_head = producer_.head.load(std::memory_order_acquire);
      cons_tail = consumer_.tail.load(std::memory_order_acquire);

      if (prod_head - cons_tail == capacity || cons_tail > prod_head) [[unlikely]] {
        // full
        return {nullptr, -1UL};
      }

      next_prod_head = prod_head + 1;
      success = producer_.head.compare_exchange_strong(prod_head, next_prod_head,
                                                       std::memory_order_release);
    } while (!success);

    return {&buffer_[prod_head & kIndexMask_], prod_head};
  }

  void CommitEnqueue(uint64_t producer_pos) {
    auto next_prod_tail = producer_pos + 1;
    while (producer_pos != producer_.tail.load(std::memory_order_relaxed)) {
      cpu_pause();
    }
    producer_.tail.store(next_prod_tail, std::memory_order_release);
  }

  std::pair<T*, uint64_t> PrepareDequeue() {
    bool success;
    uint64_t prod_tail;
    uint64_t cons_head;
    uint64_t next_cons_head;

    do {
      cons_head = consumer_.head.load(std::memory_order_acquire);
      prod_tail = producer_.tail.load(std::memory_order_acquire);

      if (cons_head >= prod_tail) {
        // empty
        return {nullptr, -1UL};
      }

      next_cons_head = cons_head + 1;
      success = consumer_.head.compare_exchange_strong(cons_head, next_cons_head,
                                                       std::memory_order_release);
    } while (!success);

    return {&buffer_[cons_head & kIndexMask_], cons_head};
  }

  void CommitDequeue(uint64_t consumer_pos) {
    auto next_cons_tail = consumer_pos + 1;
    while (consumer_pos != consumer_.tail.load(std::memory_order_relaxed)) {
      cpu_pause();
    }
    consumer_.tail.store(next_cons_tail, std::memory_order_release);
  }

  uint64_t consumer_tail() const { return consumer_.tail.load(std::memory_order_relaxed); }
  uint64_t consumer_head() const { return consumer_.head.load(std::memory_order_relaxed); }
  uint64_t producer_tail() const { return producer_.tail.load(std::memory_order_relaxed); }
  uint64_t producer_head() const { return producer_.head.load(std::memory_order_relaxed); }

 private:
  static constexpr uint64_t kIndexMask_ = capacity - 1;

  alignas(kCacheLineSize * 2) char padding_;
  alignas(kCacheLineSize * 2) Producer producer_{};
  alignas(kCacheLineSize * 2) Consumer consumer_{};

  alignas(kCacheLineSize * 2) T* buffer_ = nullptr;
};

}  // namespace xyu
