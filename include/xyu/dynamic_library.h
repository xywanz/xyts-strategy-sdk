#pragma once

#include <dlfcn.h>

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "xyu/utility.h"

namespace xyu {

class DynamicLibrary : public xyu::MoveOnly {
 public:
  explicit DynamicLibrary(const std::filesystem::path& lib_path);

  DynamicLibrary(const std::filesystem::path& libname,
                 const std::vector<std::filesystem::path>& search_paths);

  ~DynamicLibrary();

  DynamicLibrary(DynamicLibrary&& other) noexcept;

  DynamicLibrary& operator=(DynamicLibrary&& rhs) noexcept;

  void* GetSymbol(const std::string& symbol_name) noexcept(false);

  template <class R, class... Args>
  std::function<R(Args...)> GetFunction(const std::string& symbol_name) noexcept(false) {
    void* sym = GetSymbol(symbol_name);
    using FuncType = R (*)(Args...);
    return reinterpret_cast<FuncType>(sym);
  }

 private:
  void* handle_ = nullptr;
};

}  // namespace xyu
