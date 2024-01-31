#pragma once

#include <atomic>
#include <vector>

#include "xyu/memory_mapping.h"
#include "xyu/utility.h"

namespace xyts {

class SharedTradeState : public xyu::MoveOnly {
 public:
  explicit SharedTradeState(const std::string& account, bool readonly = true);

  void set_tradable(bool tradable) {
    if (!readonly_) {
      state_->store(tradable ? 0 : 1);
    }
  }
  bool tradable() const { return state_->load() == 0; }

  const std::string& filename() const { return filename_; }

 private:
  static std::string GetShmFilename(const std::string& account);

  std::string filename_;
  xyu::MemoryMapping mmaping_;
  std::atomic<uint64_t>* state_;
  bool readonly_;
};

class SharedTradeStateSet : public xyu::MoveOnly {
 public:
  explicit SharedTradeStateSet();

  bool tradable() const;

 private:
  std::vector<SharedTradeState> states_;
};

}  // namespace xyts
