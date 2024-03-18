#pragma once

#include <memory>
#include <vector>

#include "xyts/core/contract_table.h"
#include "xyts/core/market_data.h"
#include "xyts/core/trade_msg.h"
#include "xyts/strategy/strategy_context.h"

namespace xyts::strategy {

/*
  ArbitrageManager用于套利交易，它会在设定的价格上同时挂双边的主动腿，并在
  主动腿成交后自动以市价迅速完成被动腿的成交。ArbitrageManager在成交后会继
  续挂新的订单，当某一方向的套利持仓达到设定的上限时，它会暂时停止该方向上的挂
  单，另一方向的订单还会继续进行，当另一方向成交时，它又会继续该方向上的挂单，
  反复上述的过程

  一些特性：
  1. 在收盘的前20秒自动暂停交易并撤单，在开盘后恢复交易并重新挂单
  2. 会接管当前策略该交易对的所有持仓，如发现仓位不平衡会自动进行平衡
  3. 支持双边挂单，也支持单边挂单，价格设置成0则表示该方向上不挂单
  4. 支持多条被动腿

  ArbitrageManager目前已知的问题：
  1. 未对订单进行拆分，如果max_order_volume设置得过大（接近max_pos），会
     导致较多对锁的物理持仓，建议max_order_volume设置成<=0.25*max_pos。
     如果max_pos只是1手，可以忽略这个问题
  2. 只支持taker的量/maker的量是整数倍的交易对
*/

class ArbitrageManager {
 public:
  struct PositionRatio {
    ContractPtr contract;
    // 例如-1通常用于跨期套利
    int ratio;
  };

  struct Options {
    bool use_market_order_to_take = false;
    bool whole_day = false;
  };

  explicit ArbitrageManager(StrategyContext* ctx, ContractPtr maker_contract,
                            const std::vector<PositionRatio>& takers, Volume max_pos,
                            Volume max_order_volume, Options options);

  ~ArbitrageManager();

  void SetMakerPrice(double long_price, double short_price);

  void OnOrder(const OrderResponse& order);

  double GetLongMakerPrice() const;

  double GetShortMakerPrice() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace xyts::strategy
