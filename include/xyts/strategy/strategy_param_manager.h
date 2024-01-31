#pragma once

#include <filesystem>

#include "nlohmann/json.hpp"

namespace xyts::strategy {

class StrategyParamManager {
 public:
  explicit StrategyParamManager(const std::filesystem::path& param_path);

  virtual ~StrategyParamManager();

  virtual void Update(const nlohmann::json& updated_params) = 0;

  const nlohmann::json& GetJsonParams() const noexcept { return json_params_; }

  void Dump() noexcept;

 protected:
  std::filesystem::path param_path_;
  nlohmann::json json_params_;
};

}  // namespace xyts::strategy
