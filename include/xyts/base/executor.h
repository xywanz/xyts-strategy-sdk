#pragma once

#include <functional>

namespace xyts {

class Executor {
 public:
  using Func = std::function<void()>;

  virtual ~Executor() {}

  virtual void Add(Func&& f) = 0;
};

}  // namespace xyts
