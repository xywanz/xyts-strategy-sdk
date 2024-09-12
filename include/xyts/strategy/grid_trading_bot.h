#pragma once

#include "xyts/core/contract_table.h"
#include "xyts/core/market_data.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_context.h"
#include "xyts/strategy/target_position_manager.h"

namespace xyts::strategy {

enum class GridHeightType {
  kRatioLength,
  kFixedLength,
  kFixedPriceTick,
};

class GridHeight {
 public:
  GridHeight(double height, GridHeightType type) : height_(height), type_(type) {
    if (type_ != GridHeightType::kRatioLength && type_ != GridHeightType::kFixedLength &&
        type != GridHeightType::kFixedPriceTick) {
      throw std::invalid_argument("GridHeight: Invalid grid height type " +
                                  std::to_string(static_cast<int>(type_)));
    }
    if (height_ <= 0) {
      throw std::invalid_argument("GridHeight: Invalid grid height " + std::to_string(height_));
    }
  }

  double height() const { return height_; }

  GridHeightType type() const { return type_; }

 private:
  double height_;
  GridHeightType type_;
};

class GridTradingBot {
 public:
  GridTradingBot(StrategyContext* ctx, ContractPtr contract, Volume volume_per_grid,
                 GridHeight grid_height);

  GridTradingBot(StrategyContext* ctx, ContractPtr contract, Volume init_position,
                 Volume volume_per_grid, double last_place, GridHeight grid_height);

  static std::unique_ptr<GridTradingBot> Load(StrategyContext* ctx,
                                              const std::filesystem::path& path);

  void OnDepth(const DepthData& depth);

  void OnOrder(const OrderResponse& order);

  void Dump(const std::filesystem::path& path);

 private:
  void CalculateNextGridPlace();

  StrategyContext* ctx_;
  TargetPositionManager target_pos_manager_;

  ContractPtr contract_;
  Volume volume_per_grid_;
  double last_place_ = std::numeric_limits<double>::quiet_NaN();
  GridHeight grid_height_;

  double grid_up_ = std::numeric_limits<double>::max();
  double grid_down_ = 0;
  double target_position_ = 0;
};

}  // namespace xyts::strategy
