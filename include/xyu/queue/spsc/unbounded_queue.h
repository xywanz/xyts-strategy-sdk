#pragma once

#include <cassert>

#include "xyu/queue/spsc/ring_buffer.h"

namespace xyu {

// capacity是每个block的最大容量
template <class T, uint64_t capacity = 16 * 1024>
class SPSCUnboundedQueue {
  struct BlockNode {
    BlockNode() = default;

    alignas(2 * kCacheLineSize) SPSCRingBuffer<T, capacity> ring_buffer;
    alignas(2 * kCacheLineSize) std::atomic<BlockNode*> next{nullptr};
  };

 public:
  SPSCUnboundedQueue() : producer_node_(new BlockNode), consumer_node_(producer_node_) {}

  ~SPSCUnboundedQueue() {
    BlockNode* current_node = consumer_node_;
    while (current_node) {
      auto node_to_delete = current_node;
      current_node = current_node->next;
      delete node_to_delete;
    }
  }

  T* PrepareEnqueue() {
    auto p = producer_node_->ring_buffer.PrepareEnqueue();
    if (!p) [[unlikely]] {
      auto next_node = new BlockNode;
      producer_node_->next.store(next_node, std::memory_order_release);
      producer_node_ = next_node;

      p = producer_node_->ring_buffer.PrepareEnqueue();
      assert(p != nullptr && "failed to get writable address. bug");
    }
    return p;
  }

  void CommitEnqueue() { producer_node_->ring_buffer.CommitEnqueue(); }

  T* PrepareDequeue() {
    auto p = consumer_node_->ring_buffer.PrepareDequeue();
    if (!p) {
      auto next_node = consumer_node_->next.load(std::memory_order_acquire);
      if (next_node) [[unlikely]] {
        // 如果下一结点存在，再检查一下当前节点是否有数据
        p = consumer_node_->ring_buffer.PrepareDequeue();
        if (!p) {
          // 如果当前节点没有数据就可以释放了
          delete consumer_node_;
          consumer_node_ = next_node;
          p = consumer_node_->ring_buffer.PrepareDequeue();
        }
      }
    }
    return p;
  }

  void CommitDequeue() { consumer_node_->ring_buffer.CommitDequeue(); }

 private:
  alignas(2 * kCacheLineSize) char padding_;
  alignas(2 * kCacheLineSize) BlockNode* producer_node_{nullptr};
  alignas(2 * kCacheLineSize) BlockNode* consumer_node_{nullptr};
};

}  // namespace xyu
