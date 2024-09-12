#pragma once

#include <set>
#include <unordered_map>
#include <vector>

#include "xyts/core/contract_table.h"
#include "xyts/core/market_data.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

class MainContractDetector {
 public:
  explicit MainContractDetector(const std::string& symbol, std::size_t contract_required = 1,
                                std::size_t depth_required = 100, bool use_volume = false)
      : use_volume_(use_volume),
        symbol_(symbol),
        contract_required_(contract_required),
        depth_required_(depth_required) {}

  void OnDepth(const DepthData& depth);

  bool IsFinished() const { return finished_; }

  const std::vector<ContractPtr>& GetResults() const { return results_; }

 private:
  bool finished_ = false;
  bool use_volume_ = false;
  std::string symbol_;
  std::size_t contract_required_;
  std::size_t depth_required_;
  std::size_t depth_count_ = 0;
  std::unordered_map<ContractId, DepthData> depth_map_;
  std::vector<ContractPtr> results_;
};

class MultiMainContractDetector {
 public:
  explicit MultiMainContractDetector(const std::vector<std::string>& symbols,
                                     std::size_t contract_required = 1,
                                     std::size_t depth_required = 100, bool use_volume = false);

  void OnDepth(const DepthData& depth);

  bool IsFinished() const { return finished_; }

  const std::vector<ContractPtr>& GetResults(const std::string& symbol) const {
    return detectors_.at(symbol).GetResults();
  }

 private:
  std::unordered_map<std::string, MainContractDetector> detectors_;
  bool finished_ = false;
};

}  // namespace xyts::strategy
