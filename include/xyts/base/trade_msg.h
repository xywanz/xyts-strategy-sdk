#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <utility>

#include "xyts/base/error_code.h"

namespace xyts {

constexpr int kProtocolVersion = 2;
constexpr int kMinRequiredProtocolVersion = 2;
constexpr int kMaxCompatibleProtocolVersion = kProtocolVersion;

using ClientOrderId = uint64_t;
using OrderId = uint64_t;
using TickerId = uint32_t;

// 交易所的英文简称
namespace exchange {
// 上海期货交易所
constexpr char kSHFE[] = "SHFE";
// 上海国际能源交易所
constexpr char kINE[] = "INE";
// 中国金融期货交易所
constexpr char kCFFEX[] = "CFFEX";
// 郑州商品交易所
constexpr char kCZCE[] = "CZCE";
// 大连商品交易所
constexpr char kDCE[] = "DCE";
// 上海证券交易所-A股
constexpr char kSSE[] = "SH";
// 深圳证券交易所-A股
constexpr char kSZE[] = "SZ";
// 广期所
constexpr char kGFEX[] = "GFEX";
}  // namespace exchange

// 数值类型的交易所ID，方便判断及比较
enum class ExchangeId : uint32_t {
  kUnknown = 0,
  kSHFE = 1,   // 上期所
  kINE = 2,    // 上海能源交易所
  kCFFEX = 3,  // 中金所
  kCZCE = 4,   // 郑商所
  kDCE = 5,    // 大商所
  kSSE = 6,    // 上交所
  kSZE = 7,    // 深交所
  kGFEX = 8,   // 广期所
};

// 订单价格类型
enum class OrderType : uint8_t {
  kUnknown = 0,
  // 市价单，不同的平台对市价单的支持不同
  kMarket = 1,
  // 限价单，指定价格的挂单，订单时限为当日有效
  kLimit = 2,
  // 对方最优价格的限价单
  kBest = 3,
  // Fill and Kill，立即成交剩余部分立即撤单。
  // 不同平台的实现也不同，对于CTP来说是限价单，需要指定价格
  kFak = 4,
  // Fill or Kill，立即全部成交否则立即撤销。
  // 不同平台的实现也不同，对于CTP来说是限价单，需要指定价格
  kFok = 5,
  // 本方最优价格的限价单
  kBestLimit = 6,
  // 最优五档转现价
  kFal = 7,
};

// 交易的方向
enum class Direction : uint8_t {
  kUnknown = 0,
  kBuy = 1,   // 买入
  kSell = 2,  // 卖出
};

// 开平类型
enum class Offset : uint8_t {
  kUnknown = 0,
  kOpen = 1,                 // 开仓
  kClose = 1 << 1,           // 平仓
  kCloseToday = 1 << 2,      // 平今
  kCloseYesterday = 1 << 3,  // 平昨
  kAuto = 1 << 4,            // 让交易系统根据当前仓位自动选择开平
  kInternalTrade = 1 << 5,   // 子账户之间内部成交
};

// 订单状态
//
// 订单状态的转移有下面5种可能：
// 1. 被交易系统风控拒绝: rejected
// 2. 被柜台/交易所拒绝:
//       submitting -> rejected
// 3. 订单发出去，经过多次（或一次）成交之后全成:
//       submitting -> accepted -> part_traded -> (part_traded -> ...) all_traded
// 4. 订单发出去部分成交之后，剩下的撤销:
//       submitting -> accepted -> part_traded -> (part_traded -> ...) canceled
// 5. 订单发出去之后没有成交就被撤销:
//       submitting -> accepted(这个状态也可能不存在，看柜台的具体实现) -> canceled
enum class OrderStatus : uint8_t {
  kUnknown = 0,
  kSubmitting = 1,       // 正在报往交易系统
  kRejected = 1 << 1,    // 被风控/柜台/交易所等拒绝
  kAccepted = 1 << 2,    // 被交易所接收
  kPartTraded = 1 << 3,  // 部分成交，剩余部分仍处于挂单状态
  kAllTraded = 1 << 4,   // 所有成交
  kCanceled = 1 << 5,    // 被撤销（部分成交部分撤销的也属于该类型）
};

// 合约的产品类型
enum class ProductType : uint8_t {
  kUnknown = 0,
  kOptions,  // 期权
  kFutures,  // 期货
  kStock,    // 股票
  kETF,      // ETF
  kIndex,    // 指数
  kBond,     // 债券
  kFund,     // 基金
};

// balance = available + margin + frozen - floating_pnl
// total_asset = balance + floating_pnl = available + margin + frozen
// available: 可用资金，可用于购买证券资产的资金
// margin: 保证金，对于股票来说就是持有的股票资产
// fronzen: 冻结资金，未成交的订单也需要预先占用资金
struct Account {
  double total_asset;        // 总资产
  double balance;            // 结余
  double available;          // 可用资金
  double margin;             // 保证金
  double frozen;             // 冻结金额
  double commission;         // 手续费
  double floating_pnl;       // 账户浮动盈亏
  double realized_pnl;       // 当日已实现的盈亏
  std::string account_id;    // 资金账户号
  std::string account_name;  // 自定义的唯一账户名
};

// 当日历史成交记录
struct TradeRecord {
  // 收到成交回报时本地记录下的时间戳
  std::chrono::microseconds local_timestamp;
  // 交易所返回的成交时间戳
  std::chrono::microseconds exchange_timestamp;
  // 交易所返回的成交ID
  // 一般来说对某一合约而言是日内递增的，不同合约之间的成交ID并不一定递增
  uint64_t trade_sys_id;
  // 交易系统产生的唯一ID，日内递增且唯一
  uint64_t order_id;
  // 柜台或交易所产生的唯一ID，可用于在券商提供的客户端中对应到相应的订单
  uint64_t order_sys_id;
  // 方向
  Direction direction;
  // 开平标志
  Offset offset;
  // 成交量
  int volume;
  // 成交价格
  double price;
  // 内部合约名
  std::string instr;
  // 该笔成交所属的物理账户名
  std::string account_name;
  // 该笔成交所属的子账户
  std::string sub_account;
};

// 当日历史订单记录
struct OrderRecord {
  uint64_t order_id;
  uint64_t order_sys_id;
  Direction direction;
  // 开平标志
  Offset offset;
  // 订单类型
  OrderType type;
  double price;
  // 订单的原始数量
  int original_volume;
  // 已成交数量
  int traded_volume;
  // 所有成交的volume*price*contract_unit的和
  double traded_amount;
  // 订单状态
  OrderStatus status;
  // 错误码，如果有错误的话，错误码参照xyts/base/error_code.h
  ErrorCode error_code;
  // 发出订单的本地时间戳
  std::chrono::microseconds insert_timestamp;
  // 收到订单回报的本地时间戳
  std::chrono::microseconds accepted_timestamp;
  // 撤销发出的本地时间戳
  std::chrono::microseconds cancel_timestamp;
  // 订单完成的本地时间戳
  std::chrono::microseconds completion_timestamp;

  std::string instr;
  std::string account_name;
  std::string sub_account;
};

struct InitialYdPosition {
  std::string ticker;
  int long_volume;
  int short_volume;
};

// 下面是strategy和trading_server之间的交互协议
// strategy通过IPC向trading_server发送交易相关指令
// 用于简单验证交易协议的合法性
inline const uint32_t kTradingCmdMagic = 0x1709394;

// 交易指令类型
enum class TraderCmdType : uint32_t {
  kNewOrder = 1,       // 新订单
  kCancelOrder,        // 撤单
  kCancelTick,         // 撤销某个合约的所有订单
  kCancelAll,          // 撤销所有订单
  kResetOrderTimeout,  // 重置订单超时时间
};

// 订单请求
struct TraderOrderReq {
  uint64_t client_order_id;  // 策略端生成的订单ID
  uint32_t ticker_id;
  Direction direction;
  Offset offset;
  OrderType type;
  int volume;
  double price;
  std::chrono::microseconds timeout;
  uint64_t user_data;  // 策略自定义数据
};

// 撤单请求
struct TraderCancelReq {
  uint64_t order_id;
};

// 撤单请求
struct TraderCancelTickerReq {
  uint32_t account_index;
  uint32_t ticker_id;
};

// 撤单请求
struct TraderCancelAllReq {
  uint32_t account_index;
};

// 重置订单超时
struct TraderResetOrderTimeoutReq {
  uint64_t order_id;
  std::chrono::microseconds timeout;
};

// 通知
struct TraderNotification {
  uint64_t signal;
};

// 交易指令
struct TraderCommand {
  uint32_t magic;  // 简单验证指令的合法性
  TraderCmdType type;
  bool without_check;
  int conn_id;  // 用于在Trader找到具体策略实例
  union {
    TraderOrderReq order_req;
    TraderCancelReq cancel_req;
    TraderCancelTickerReq cancel_ticker_req;
    TraderCancelAllReq cancel_all_req;
    TraderResetOrderTimeoutReq reset_order_timeout_req;
    TraderNotification notification;
  };
};

// 订单回报
struct OrderResponse {
  // 策略生成的订单号，即SendOrder/Buy/Sell返回的那个订单号
  // 每次运行时id唯一，多次重启之间并不保证唯一，不同策略之间也不保证唯一
  uint64_t client_order_id;
  // trader生成的订单号，日内唯一递增，策略间保持唯一，重启后也能保证唯一
  uint64_t order_id;
  // 合约ID
  uint32_t ticker_id;
  // 交易方向
  Direction direction;
  // 开平标志
  Offset offset;
  // 订单最新状态
  OrderStatus status;
  // 错误码，如果被拒单的话
  ErrorCode error_code;
  // 委托价格
  double price;
  // 原始委托数量
  int original_volume;
  // 已成交数量，剩余未成交数量 = original_volume - traded_volume
  int traded_volume;
  // 用户自定义数据，发单时填写，回报带回
  uint64_t user_data;

  // 只有本次发生了成交才会有下面三个字段
  // 本次成交数量
  // FIXME: uint32_t?
  uint32_t this_traded;
  // 本次成交价格
  double this_traded_price;
  // 本次成交的交易所时间戳
  std::chrono::microseconds trade_timestamp;  // 在算法交易回测加速中，统计每笔单完成时间

  std::chrono::microseconds local_timestamp;

  // 订单在xyts交易系统中是否还活着
  bool IsActive() const {
    return static_cast<uint8_t>(status) & (static_cast<uint8_t>(OrderStatus::kSubmitting) |
                                           static_cast<uint8_t>(OrderStatus::kAccepted) |
                                           static_cast<uint8_t>(OrderStatus::kPartTraded));
  }
  // 是否是正报状态，即还未收到交易所回报
  bool IsSubmitting() const { return status == OrderStatus::kSubmitting; }
  // 订单是否仍挂在交易所队列
  bool IsOnExchange() const {
    return static_cast<uint8_t>(status) & (static_cast<uint8_t>(OrderStatus::kAccepted) |
                                           static_cast<uint8_t>(OrderStatus::kPartTraded));
  }
  bool IsAllTraded() const { return status == OrderStatus::kAllTraded; }
  bool IsCanceled() const { return status == OrderStatus::kCanceled; }
  bool IsError() const { return status == OrderStatus::kRejected; }
  bool IsPositionNotEnough() const {
    return status == OrderStatus::kRejected && error_code == ErrorCode::kPositionNotEnough;
  }

  int ActiveVolume() const { return IsActive() ? original_volume - traded_volume : 0; }
};

// 物理持仓
struct PositionData {
  uint32_t ticker_id;
  int long_volume;
  int long_yd_volume;
  int short_volume;
  int short_yd_volume;
};

// 逻辑持仓
struct LogicalPositionData {
  uint32_t ticker_id;
  int volume;
};

// 策略从Trader收到的消息的类型
enum StrategyMsgType : uint32_t {
  kOrderResponse,
  kPhysicalPosition,
  kLogicalPosition,
};

// 策略从Trader收到的消息
struct StrategyMsg {
  StrategyMsgType msg_type;
  union {
    OrderResponse order_rsp;
    PositionData physical_position;
    LogicalPositionData logical_position;
  };
};

struct ChannelSubscriptionInfo {
  uint16_t channel_id;
  uint16_t data_type;
};

constexpr int kChannelShift = 16;

using Fill = TradeRecord;

static_assert(std::is_trivially_copyable_v<TraderCommand>);
static_assert(std::is_trivially_copyable_v<StrategyMsg>);

}  // namespace xyts
