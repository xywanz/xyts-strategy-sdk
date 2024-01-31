#pragma once

#include <dlfcn.h>

#include <functional>
#include <string>

namespace xyu {

class DynamicLibrary {
 public:
  explicit DynamicLibrary(const std::string& lib_path);
  ~DynamicLibrary();

  DynamicLibrary(const DynamicLibrary&) = delete;
  DynamicLibrary(DynamicLibrary&& other) noexcept;

  DynamicLibrary& operator=(const DynamicLibrary&) = delete;
  DynamicLibrary& operator=(DynamicLibrary&& rhs) noexcept;

  void Swap(DynamicLibrary& other) noexcept;

  void* GetSymbol(const std::string& symbol_name);

  template <class R, class... Args>
  std::function<R(Args...)> GetFunction(const std::string& symbol_name) {
    void* sym = GetSymbol(symbol_name);
    if (sym) {
      using FuncType = R (*)(Args...);
      return reinterpret_cast<FuncType>(sym);
    }
    return nullptr;
  }

  std::string last_error() const;

 private:
  void* handle_ = nullptr;
  std::string last_err_;
};

}  // namespace xyu
