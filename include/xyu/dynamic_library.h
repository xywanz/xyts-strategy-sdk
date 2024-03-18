#pragma once

#include <dlfcn.h>

#include <functional>
#include <string>

#include "xyu/utility.h"

namespace xyu {

class DynamicLibrary : public xyu::MoveOnly {
 public:
  explicit DynamicLibrary(const std::string& lib_path);
  ~DynamicLibrary();

  DynamicLibrary(DynamicLibrary&& other) noexcept;

  DynamicLibrary& operator=(DynamicLibrary&& rhs) noexcept;

  void Swap(DynamicLibrary& other) noexcept;

  void* GetSymbol(const std::string& symbol_name) noexcept(false);

  template <class R, class... Args>
  std::function<R(Args...)> GetFunction(const std::string& symbol_name) noexcept(false) {
    void* sym = GetSymbol(symbol_name);
    using FuncType = R (*)(Args...);
    return reinterpret_cast<FuncType>(sym);
  }

  void Close() noexcept;

 private:
  void* handle_ = nullptr;
};

}  // namespace xyu
