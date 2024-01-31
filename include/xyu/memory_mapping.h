#pragma once

#include <string>

#include "xyu/file.h"

namespace xyu {

class MemoryMapping {
 public:
  struct Options {
    Options() {}

    Options& set_shared(bool v) {
      shared = v;
      return *this;
    }
    Options& set_readable(bool v) {
      readable = v;
      return *this;
    }
    Options& set_writable(bool v) {
      writable = v;
      return *this;
    }
    Options& set_grow(bool v) {
      grow = v;
      return *this;
    }

    void* address = nullptr;
    bool shared = true;
    bool readable = true;
    bool writable = false;
    bool grow = false;
  };

  static Options writable() { return Options().set_writable(true).set_grow(true); }

  MemoryMapping() = delete;
  explicit MemoryMapping(const char* file, off_t offset = 0, off_t length = -1,
                         Options options = Options());
  explicit MemoryMapping(const std::string& file, off_t offset = 0, off_t length = -1,
                         Options options = Options());

  MemoryMapping(const MemoryMapping&) = delete;
  MemoryMapping(MemoryMapping&& other) noexcept;

  ~MemoryMapping();

  MemoryMapping& operator=(const MemoryMapping&) = delete;
  MemoryMapping& operator=(MemoryMapping&& other);

  void swap(MemoryMapping& other) noexcept;

  bool mlock();
  void munlock();

  char* data() const { return reinterpret_cast<char*>(data_); }

  std::size_t length() const { return std::size_t(length_); }

  bool locked() const { return locked_; }

 private:
  File file_{};
  void* map_start_ = nullptr;
  off_t map_length_ = 0;
  void* data_ = nullptr;
  off_t length_ = 0;
  bool locked_ = false;
  Options options_{};
};

}  // namespace xyu
