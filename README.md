# 策略开发教程

联系方式: kevin.lau.gd@gmail.com

## 开发环境

```sh
$ lsb_release -a
No LSB modules are available.
Distributor ID:	Ubuntu
Description:	Ubuntu 22.04.3 LTS
Release:	22.04
Codename:	jammy

g++ --version
g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
```

会继续升级至完全支持c++20的g++版本，尽可能跟进最新的g++版本，后续会考虑打包成docker镜像

几点说明：
- 不支持其他平台，即使使用g++ 11.3.0也可能会有问题。可考虑支持Clang及AppleClang
- 不支持图形界面，但提供数据推送的接口，有需要可自己对接
- 交易系统使用O3优化
- 每个接口或模块尽可能做成可测试的，单元测试尽可能覆盖所有代码
- 回测暂时不支持pystrategy，后续会加上
- 支持接收沪深逐笔数据，且提供了沪深逐笔实时合成更高频率快照（在盘口的基础上还提供了更丰富的逐笔因子）的模块
- 高频的强化学习Env即将上线

## 简介

策略的开发主要涉及到三个核心的类型
- `xyts::strategy::Strategy`
- `xyts::strategy::StrategyContext`
- `xyts::strategy::StrategyParamManager`

Strategy包含了一系列的虚回调函数，需要用户去继承并将策略逻辑实现在相应的回调函数里

StrategyContext是策略运行的上下文环境，用于策略跟实盘交易/回测环境的交互，提供了下单、撤单、订阅行情、查询成交、查询持仓、添加定时器等策略需要用到的功能

StrategyParamManager用于策略读写参数，并支持运行时从外部对策略参数进行修改、实时落地同步到json文件。用户需要按照指定格式为每个策略编写一个json文件，该文件用于定义策略参数，然后用脚本strategy_param_mgr_generator.py从该配置生成.h头文件，该头文件提供了策略访问策略参数的接口

一些特性：

- 回测和实盘使用同一套策略代码，一次编译生成的策略动态库既可用于回测，也可用于实盘
- 实盘采用多进程架构，通过共享内存传递消息，各个策略可随时启停。策略之间互不影响，但可通过消息队列来订阅发布消息

一些缺陷：

- 为了不增加额外的负担，StrategyContext提供的函数都不是线程安全的，只能够用在策略构造函数、析构函数以及各类回调（包括通过ctx添加的定时器）中。如果需要使用多线程，建议通过线程安全的消息队列传递消息，策略通过定时任务定期处理消息
- 回测程序并不能很好地支持用户自己创建的多线程，需要用户自己来保证其余线程时间上的同步

## 典型策略程序示例

以一个常见的跨期套利策略为例，来说明策略的整体开发流程。这个策略并不包含错误处理，我们假设订单最后都能成功报出去。假设我们的策略名字叫做strategy_spread_arb

首先创建策略的配置文件strategy_spread_arb.json，我们在配置文件中定义了四个策略参数，分别是套利两条腿的合约名、套利上下轨的位置
```json
{
  "leg1_instr": {
    "type": "string",
    "value": "FUT_SHFE_rb-202405"
  },
  "leg2_instr": {
    "type": "string",
    "value": "FUT_SHFE_rb-202410"
  },
  "upper_line": {
    "type": "double",
    "value": 30
  },
  "lower_line": {
    "type": "double",
    "value": 20
  }
}
```

接着用strategy_param_mgr_generator.py来生成C++的策略参数访问接口

```sh
python3 strategy_param_mgr_generator.py strategy_spread_arb.json
```

运行后会在当前目录生成strategy_spread_arb.h，该文件内容如下

```cpp
//Generated from strategy_spread_arb.json. DO NOT EDIT!
#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "xyts/strategy/strategy_param_manager.h"

class StrategySpreadArbParamManager final : public ::xyts::strategy::StrategyParamManager {
 public:
  explicit StrategySpreadArbParamManager(const std::filesystem::path& param_path): StrategyParamManager(param_path) {
    CheckParam("leg1_instr");
    leg1_instr = json_params_["leg1_instr"]["value"].get<std::string>();
    CheckParam("leg2_instr");
    leg2_instr = json_params_["leg2_instr"]["value"].get<std::string>();
    CheckParam("upper_line");
    upper_line = json_params_["upper_line"]["value"].get<double>();
    CheckParam("lower_line");
    lower_line = json_params_["lower_line"]["value"].get<double>();
  }

  const std::string& get_leg1_instr() const { return leg1_instr; }
  void set_leg1_instr(const std::string& _leg1_instr) {
    leg1_instr = _leg1_instr;
    json_params_["leg1_instr"]["value"] = _leg1_instr;
  }
  const char* nameof_leg1_instr() const { return "leg1_instr"; }


  const std::string& get_leg2_instr() const { return leg2_instr; }
  void set_leg2_instr(const std::string& _leg2_instr) {
    leg2_instr = _leg2_instr;
    json_params_["leg2_instr"]["value"] = _leg2_instr;
  }
  const char* nameof_leg2_instr() const { return "leg2_instr"; }


  const double get_upper_line() const { return upper_line; }
  void set_upper_line(const double _upper_line) {
    upper_line = _upper_line;
    json_params_["upper_line"]["value"] = _upper_line;
  }
  const char* nameof_upper_line() const { return "upper_line"; }


  const double get_lower_line() const { return lower_line; }
  void set_lower_line(const double _lower_line) {
    lower_line = _lower_line;
    json_params_["lower_line"]["value"] = _lower_line;
  }
  const char* nameof_lower_line() const { return "lower_line"; }


  //增量更新, 只更新readonly=false的元素
  void Update(const nlohmann::json& update_json) final {
    if (update_json.contains("leg1_instr") && update_json["leg1_instr"].contains("value")) {
      auto val = update_json["leg1_instr"]["value"].get<std::string>();
      set_leg1_instr(val);
    }
    if (update_json.contains("leg2_instr") && update_json["leg2_instr"].contains("value")) {
      auto val = update_json["leg2_instr"]["value"].get<std::string>();
      set_leg2_instr(val);
    }
    if (update_json.contains("upper_line") && update_json["upper_line"].contains("value")) {
      auto val = update_json["upper_line"]["value"].get<double>();
      set_upper_line(val);
    }
    if (update_json.contains("lower_line") && update_json["lower_line"].contains("value")) {
      auto val = update_json["lower_line"]["value"].get<double>();
      set_lower_line(val);
    }

  }

 private:
  void CheckParam(const char* param_name) {{
    if (!json_params_.contains(param_name) || !json_params_[param_name].contains("value")) {{
      throw std::runtime_error("Init strategy params failed: Param '" + std::string(param_name) + "' not found in *autogen.json");
    }}
  }}

  std::string leg1_instr;
  std::string leg2_instr;
  double upper_line = 0;
  double lower_line = 0;
};
```

我们在策略代码中可以通过StrategySpreadArbParamManager这个类来访问刚刚我们在json文件中定义的4个参数。这个类的实例是如何构造出来的我们不需要管，因为StrategyContext会自动帮我们构造好，我们只需要知道如何使用即可，这个会在策略实现部分继续说明。

接下来我们看看策略的实现部分，创建策略源文件strategy_spread_arb.cpp
```cpp
// 以下三个头文件中包含的分别是我们刚刚提到的三个核心类型
#include "xyts/strategy/strategy.h"
#include "xyts/strategy/strategy_context.h"
#include "strategy_spread_arb_param_manager.h"
// 日志库
#include "xyts/core/log.h"
// 合约表
#include "xyts/core/contract_table.h"
// 项目里还有很多实用的库
// #include "xyu/datetime.h"  Python-like datetime库
// ...

using namespace xyts;
using namespace xyts::strategy;

class StrategySpreadArb final : public Strategy {
 public:
  // 构造函数必须如下所示，只传入一个StrategyContext指针。
  // 我们需要将ctx保存下来，用于接下来与交易环境进行交互。
  // 构造函数就相当于策略的初始化，构造策略时策略上下文已经完成了初始化。
  explict StrategySpreadArb(StrategyContext* ctx);

  ~StrategySpreadArb();

  void OnTick(const TickData& tick) final;

  // 订单状态有更新
  void OnOrder(const OrderResponse& order) final;

 private:
  StrategyContext* ctx_;                  // 用于保存StrategyContext
  StrategySpreadArbParamManager* param_;  // 策略参数

  ContractPtr leg1_contract_;
  ContractPtr leg2_contract_;

  double leg1_mid_price_ = std::numeric_limits<double>::quiet_NaN();
  double leg2_mid_price_ = std::numeric_limits<double>::quiet_NaN();
};

StrategySpreadArb::StrategySpreadArb(StrategyContext* ctx)
    : ctx_(ctx),
      param_(dynamic_cast<StrategySpreadArbParamManager*>(ctx->GetParamManager())),
      leg1_contract_(ContractTable::GetByInstrument(param_->get_leg1_instr())),
      leg2_contract_(ContractTable::GetByInstrument(param_->get_leg2_instr())) {
  if (!leg1_contract_) {
    throw std::runtime_error("Unknown leg1 {}" + param_->get_leg1_instr());
  }
  if (!leg2_contract_) {
    throw std::runtime_error("Unknown leg2 {}" + param_->get_leg2_instr());
  }

  // 订阅leg1和leg2的行情和持仓信息
  ctx_->Subscribe({leg1_contract_->instr, leg2_contract_->instr});
}

StrategySpreadArb::~StrategySpreadArb() {
  LOG_INFO("Strategy {} stopped", ctx_->GetStrategyName());
  // 可以在析构函数中保存策略的一些状态到文件中，用于下次启动时加载
  // do sth.
}

void StrategySpreadArb::OnTick(const TickData& tick) {
  // 竞价阶段bid[0]==ask[0]，我们不在该阶段交易
  if (std::abs(tick.bid[0] - tick.ask[0]) < 1e-6) {
    return;
  }

  // 这里没有考虑bid或ask没有挂单的情况，假设bid和ask价格都存在
  if (tick.contract_id == leg1_contract_->contract_id) {
    leg1_mid_price_ = (tick.bid[0] + tick.ask[0]) / 2;
  } else {
    // 为了简单，只在收到leg2行情的时候计算spread
    leg2_mid_price_ = (tick.bid[0] + tick.ask[0]) / 2;
    // leg1还没收到行情，无法计算spread
    if (std::isnan(leg1_mid_price_)) {
      return;
    }
    double spread = leg1_mid_price_ - leg2_mid_price_;
    auto pos = ctx_->GetLogicalPosition(tick.contract_id);
    if (spread >= param_->get_upper_line()) {
      // 超过上轨，如果仓位还没满做空spread
      if (pos.volume > -1) {
        ctx_->Buy(leg2_contract_->contract_id, 1, OrderType::kLimit,
                  leg2_contract_->upper_limit_price, std::chrono::microseconds{100 * 1000});
        ctx_->Sell(leg1_contract_->contract_id, 1, OrderType::kLimit,
                   leg1_contract_->lower_limit_price, std::chrono::milliseconds{100 * 1000});
      }
    } else if (spread <= param_->get_lower_line()) {
      // 跌破下轨，如果仓位还没满则做多spread
      if (pos.volume < 1) {
        ctx_->Sell(leg2_contract_->contract_id, 1, OrderType::kLimit,
                   leg2_contract_->lower_limit_price std::chrono::milliseconds{100 * 1000});
        ctx_->Buy(leg1_contract_->contract_id, 1, OrderType::kLimit,
                  leg1_contract_->upper_limit_price std::chrono::milliseconds{100 * 1000});
      }
    }
  }
}

void StrategySpreadArb::OnOrder(const OrderResponse& order) {
  const auto* contract =
      order.contract_id == leg1_contract_->contract_id ? leg1_contract_ : leg2_contract_;
  LOG_INFO("{} {}{} {} {} px:{:.2f} fill/total:{}/{} order_id:{}", contract->instr, order.direction,
           order.offset, order.status, order.error_code, order.price, order.accum_trade_volume,
           order.original_volume, order.order_id);

  if (order.current_trade_volume > 0) {
    LOG_INFO("{} {}{} {:.2f}@{}", contract->instr, order.direction, order.offset,
             order.current_trade_price, order.current_trade_volume);
  }
}

// 导出策略符号，使得能通过动态库的形式加载
EXPORT_STRATEGY(StrategySpreadArb);
```

编写完后将策略编译成动态库即可被实盘或回测环境加载

## 策略程序编译

参考`https://github.com/xywanz/xyts-strategy-demo`

## 策略程序回测

我们使用backtester程序对策略进行回测

我们需要准备以下数据或程序用于回测，目录结构如下

```sh
├── bin
│   └── backtester
├── conf
│   └── backtester.yaml
├── data
├── lib
│   ├── libstrategy_spread_arb.so
│   └── libxyts.so
└── log
```

我们在conf下创建backtester.yaml，并填入如下配置

```yaml
strategies:
  - strategy_name: strategy_spread_arb
    strategy_so: ../lib/libstrategy_spread_arb.so  # 编译好的策略动态库
    strategy_param_file: ../conf/strategy_spread_arb.json
# 回测范围为[begin_date, end_date]之间的所有交易日
begin_date: 2024-01-01
end_date: 2024-01-15
trading_at_night: true  # 是否交易夜盘
night_start_time: '20:55'  # 夜盘策略启动时间
day_start_time: '08:55'  # 日盘策略启动时间
# 夜盘交易时段
night_sessions:
  - begin_time: '20:59'
    end_time: '23:01'
# 日盘交易时段
day_sessions:
  - begin_time: '09:00'
    end_time: '10:15'
  - begin_time: '10:30'
    end_time: '11:30'
  - begin_time: '13:30'
    end_time: '15:01'
initial_fund: 100000000  # 起始资金
contract_file: ../data/contracts_%Y%m%d.csv  # xydata合约表路径
holiday_dir: ../data/holiday  # xydata交易日历路径
# 行情数据配置
data_feed:
  name: csv_data_feed
  data_dir: ../data/tick  # xydata csv格式的快照数据目录
matching_engine:
  name: simple_matching_engine  # simple为对价撮合
# 手续费配置
commission:
- instr: 'FUT_SHFE_rb.+?'
  type: percentage
  rate: 0.0001
# 滑点
slippage: 0
```

准备回测需要用到的交易日历、快照以及合约表数据，放在data目录中。数据如何组织以及数据的格式请参考`xydata`

准备好数据后，进入到bin目录中，执行命令启动回测

```sh
nohup ./backtester ../conf/backtester.yaml ../log/backtester.log 2>&1 &
```

日志会输出到log目录下，回测完成后会将回测结果输出到backtest_stats.json

## 策略实盘上线

实盘用strategy_loader来加载我们编译好的策略动态库。

在启动策略前需要启动以下进程：

- trader
- market_center
- 各个data_feed程序用于收行情（只需要在market_center之后启动，与策略之间的启动顺序没有要求，而且可以随时启停），如果有多个源的话可以启动多个以提高可靠性

配置好交易系统及策略后，可以在bin目录下执行以下命令启动交易系统
```sh
# 生成合约表
./trader gen_contract_table ../log/trader.log ../conf
# 一键启动trader、market_center以及各个行情gateway
./restart.sh
# 启动策略
python3 manager.py start_strategy strategy_spread_arb_01
```

## Strategy基类

策略是由事件驱动，策略目前支持以下几种事件

- Tick
- Order

### OnTick

如果策略在初始化时订阅了一些合约，那么这些合约的行情更新时会通过OnTick通知策略，策略可在OnTick中实现策略的主要逻辑

### OnOrder

策略订单的状态发生任何变动时，都会通过OnOrder通知策略，具体的变动包括

- 订单被XYTS风控拒绝
- 订单被XYTS接收
- 订单被柜台/交易所拒绝
- 订单被交易所接收
- 订单被撤销
- 订单发生了成交（每有一笔成交都会通知一次）

### OnUpdatingParam

策略参数由外部进行修改时，会由该回调通知，消息中只包含了那些被修改的参数。假如策略的upper_line和lower_line两个参数被外部修改了，消息格式如下

```json
{
  "upper_line": {
    "value": 32
  },
  "lower_line": {
    "value": 22
  }
}
```

### OnMessage

如果策略在初始化时调用了SubscribeTopics订阅了一些topics的话，如果该topics中有策略感兴趣数据进来，会通过OnMessage通知策略，策略需要自行对数据进行解包

也可以通过OnMessage接收来自data_feed_api的自定义行情

### OnCommand

策略收到自定义控制命令

## StrategyContext用法说明

### AddTimeout & AddPeriodicCallback

XYTS内置高精度定时器功能，可以在策略构造时或任意回调里使用，并支持取消。该功能完美支持回测。

单次的超时回调

```cpp
// 500微秒后执行1次
auto timer_id = ctx->AddTimeout(std::chrono::microseconds{500}, [this](auto) {
  // do sth.
});
// 如果不想在500微秒后执行了，可以移除定时器
ctx->RemoveTimer(timer_id);
// 可以支持多种时间类型
ctx->AddTimeout(std::chrono::milliseconds{1}, [this](auto) {});
ctx->AddTimeout(std::chrono::seconds{1}, [this](auto) {});
```

定期执行的回调

```cpp
// 每500微秒执行1次
auto timer_id = ctx->AddPeriodicCallback(std::chrono::microseconds{500}, [this](auto id) {
  // do sth.
  // 不需要了的话既可以在外面取消，也可以在内部取消
  // ctx->RemoveTimer(id);
});
// 如果不需要了，可以移除定时器
ctx->RemoveTimer(timer_id);
// 可以支持多种时间类型
ctx->AddPeriodicCallback(std::chrono::milliseconds{1}, [this](auto) {});
ctx->AddPeriodicCallback(std::chrono::seconds{1}, [this](auto) {});
```

### Subscribe

Subscribe用于订阅行情和实时持仓更新信息，一般在策略构造函数中调用，策略初始化时就应该知道该订阅哪些合约，不建议在策略运行过程中调用

```cpp
MyStrategy::MyStrategy(StrategyContext* ctx) {
  ctx->Subscribe({"FUT_SHFE_rb-202405", "FUT_SHFE_rb-202410"});  // 支持指定具体合约
  ctx->Subscribe({"FUT_SHFE_rb.+?"});  // 支持使用正则表达式，匹配规则与std::regex相同
  ctx->Subscribe({"FUT_SHFE_rb.+?", "FUT_SHFE_hc-202405"});  // 可以混合使用
}
```

### SubscribeTopics

XYTS提供了基于共享内存的低延迟消息队列，可以在策略构造函数中订阅感兴趣的channel和data_type，之后如有新消息会通过OnMessage进行推送

```cpp
constexpr uint16_t my_channel_id = 888;
constexpr uint16_t my_data_type = 1;
ctx->SubscribeTopics({{my_channel_id, my_data_type}})
```

### RegisterPublisher

如果策略需要往消息队列发送消息，需要在策略构造时调用该函数进行注册，未注册的策略发送的消息将无法被消费

```cpp
ctx->RegisterPublisher()
```

### GetWallTime

获取当前微秒精度的时间戳，实盘时返回的是当前时间，回测返回的回测时模拟的时间。尽可能使用该接口来获取时间，以确保实盘和回测行为能保持一致。另外，GetWallTime可配合xyu/datetime.h使用以获取更多功能

```cpp
#include "xyu/datetime.h"

namespace dt = xyu::datetime;

// ...

auto ts = ctx->GetWallTime();
auto now = dt::datetime::fromtimestamp(ts);
LOG_INFO("{}", now.strftime("%Y-%m-%d %H:%M:%S.%f"));
```

### Alarm

向外推送告警信息，可支持推送至企业微信

```cpp
ctx->Alarm("Hello");
```

### Stop

停止策略并退出程序，用于遇到BUG或是其他紧急情况下策略内部主动退出

```cpp
ctx->Stop();
```

### SendOrder

支持订单超时参数，超时会自动撤销订单

```cpp
auto contract = ContractTable::GetByInstrument("FUT_SHFE_rb-202405");
// 设置了100ms的超时
// 返回的client_order_id可对订单进行操作，如撤单、修改订单超时等，也可以用于在回报中识别订单
auto cli_order_id = ctx->SendOrder(contract->contract_id, 1, Direction::kBuy, Offset::kAuto,
                                   OrderType::kLimit, 3900, std::chrono::microseonds{100 * 1000}); 
```

### Buy

对SendOrder的简单封装，Offset为Offset::kAuto

```cpp
auto contract = ContractTable::GetByInstrument("FUT_SHFE_rb-202405");
ctx->Buy(contract->contract_id, 1, OrderType::kLimit, 3900);
```

### Sell

对SendOrder的简单封装，Offset为Offset::kAuto

```cpp
auto contract = ContractTable::GetByInstrument("FUT_SHFE_rb-202405");
ctx->Sell(contract->contract_id, 1, OrderType::kLimit, 3900);
```

### CancelOrder

撤单，调用一次即可保证订单立刻结束

```cpp
auto contract = ContractTable::GetByInstrument("FUT_SHFE_rb-202405");
auto cli_order_id = ctx->SendOrder(contract->contract_id, 1, Direction::kBuy, Offset::kAuto,
                                   OrderType::kLimit, 3900, std::chrono::microseonds{100 * 1000});
ctx->CancelOrder(cli_order_id);
```

### ResetOrderTimeout

重置订单超时时间

```cpp
auto contract = ContractTable::GetByInstrument("FUT_SHFE_rb-202405");
auto cli_order_id = ctx->SendOrder(contract->contract_id, 1, Direction::kBuy, Offset::kAuto,
                                   OrderType::kLimit, 3900, std::chrono::microseonds{100 * 1000});
// 重新设置成1s，超时是从设置的那一刻开始计算
ctx->ResetOrderTimeout(cli_order_id, std::chrono::microseconds{1000 * 1000});
```

### GetPosition

获取单个合约的实时物理持仓，效率很高

```cpp
auto contract = ContractTable::GetByInstrument("FUT_SHFE_rb-202405");
auto pos = ctx->GetPosition(contract->contract_id);
```

### GetPositions

查询所有物理持仓，因为涉及到拷贝，如果合约量较大性能会稍差，如果需要经常用到批量查询且对性能有要求，建议通过OnPosition回调自己维护

```cpp
auto positions = ctx->GetPositions();
for (const auto& position : positions) {
  // ...
}
```

### GetLogicalPosition

获取单个合约的实时逻辑持仓，效率很高

```cpp
auto contract = ContractTable::GetByInstrument("FUT_SHFE_rb-202405");
auto pos = ctx->GetLogicalPosition(contract->contract_id);
```

### GetLogicalPositions

查询所有逻辑持仓，因为涉及到拷贝，如果合约量较大性能会稍差，如果需要经常用到批量查询且对性能有要求，建议通过OnPosition回调自己维护

```cpp
auto positions = ctx->GetLogicalPositions();
for (const auto& position : positions) {
  // ...
}
```

### GetFills

从数据库读取策略当日成交，性能较差，只适合在策略初始化时查询。如果要维护实时的成交列表，需要配合OnOrder回调使用

```cpp
auto fills = ctx->GetFills();  // 获取策略所有成交

auto contract = ContractTable::GetByInstrument("FUT_SHFE_rb-202405");
auto rb2405_fills = ctx->GetFills(contract->contract_id);  // 获取策略在rb2405上的成交
```

### GetAccount

查询帐户资金信息，这个一般用得比较少，需要填入配置的帐户名称

```cpp
auto account = GetAccount("my_ctp_account");
```

### PublishMessage

在初始化时调用了RegisterPublisher的策略可以通过PublishMessage往消息队列推送消息

data的最大长度有限制，定义在xyts/core/market_data.h的kMaxTopicMessageLen，超出的话接口会抛异常

```cpp
const char* data = "hello, world";
ctx->PublishMessage(channel_id, data_type, data, strlen(data) + 1, version);
```

### GetStrategyName

获取当前策略实例的名称

```cpp
LOG_INFO("{}", ctx->GetStrategyName());
```

### GetParamManager

获取策略参数对象，通常需要用dynamic_cast转换成相应的派生类型，参考`典型策略程序示例`这一章节的示例

## 高级订单管理功能

高级订单管理功能提供了如套利、追踪止损、条件单等功能

### OrderManager

OrderManager能同时在两个价位上分别挂上买单和卖单

```cpp
#include "xyts/strategy/order_manager.h"
// include other necessary header files

class MyStrategy final : public Strategy {
 public:
  explicit MyStrategy(StrategyContext* ctx)
      : ctx_(ctx),
        param_(dynamic_cast<MyStrategyParamManager*>(ctx->GetParamManager())),
        order_manager(ctx_) {
    ctx_->Subscribe({param_->get_instr()});
  }

  void OnTick(const TickData& tick) final {
    // 每次行情更新都会先撤销掉原来的订单，并在买1和卖1分别挂一手订单
    order_manager_.PlaceOrder(tick.contract_id, 1, tick.bid[0], 1, tick.ask[0]);
  }

  void OnOrder(const OrderResponse& rsp) final {
    order_manager_.OnOrder(rsp);
  }

 private:
  StrategyContext ctx_;
  MyStrategyParamManager* param_;
  OrderManager order_manager_;
};
```

### ArbitrageManager

ArbitrageManager用于套利交易，它会在设定的价格上同时挂双边的主动腿，并在主动腿成交后自动以市价迅速完成被动腿的成交。ArbitrageManager会按照指定的数量和价格持续不断地在双边进行挂单。

```cpp
#include "xyts/strategy/arbitrage_manager.h"
// include other necessary header files

class MyStrategy final : public Strategy {
 public:
  explicit MyStrategy(StrategyContext* ctx)
      : ctx_(ctx),
        param_(dynamic_cast<MyStrategyParamManager*>(ctx->GetParamManager())),
        maker_contract_(ContractTable::GetByInstrument(param_->get_maker_instr())),
        taker_contract_(ContractTable::GetByInstrument(param_->get_taker_instr())),
        arb_manager_(ctx_, maker_contract_,
                     std::vector<ArbitrageManager::PositionRatio>{{taker_contract_, -1}}, 10, 1) {
    ctx_->Subscribe({param_->get_maker_instr(), param_->get_taker_instr()});
  }

  void OnTick(const TickData& tick) final {
    // 价格设置为0表示取消并暂停该方向上的挂单
    // 如果存在套利机会，挂出订单
    if (xxx) {
      arb_manager_.SetMakerPrice(long_price, short_price);
    }
  }

  void OnOrder(const OrderResponse& rsp) final {
    arb_manager_.OnOrder(rsp);
  }

 private:
  StrategyContext ctx_;
  MyStrategyParamManager* param_;
  ContractPtr maker_contract_;
  ContractPtr taker_contract_;
  ArbitrageManager arb_manager_;
};
```

### TargetPositionManager

策略通过TargetPositionManager设置目标持仓量后，TargetPositionManager会自动将持仓调整到目标持仓量，策略无需再处理订单逻辑。

```cpp
#include "xyts/strategy/target_position_manager.h"
// include other necessary header files

class MyStrategy final : public Strategy {
 public:
  explicit MyStrategy(StrategyContext* ctx)
      : ctx_(ctx),
        param_(dynamic_cast<MyStrategy*>(ctx->GetParamManager())),
        contract_(ContractTable::GetByInstrument(param_->get_instr())),
        target_pos_manager_(ctx_) {
    ctx_->Subscribe({param_->get_instr()});
  }

  void OnTick(const TickData& tick) final {
    if (tick.last_price > 3900) {
      target_pos_maager_.SetTargetPosition(tick.contract_id, -10);
    } else if (tick.last_price < 3880) {
      target_pos_maager_.SetTargetPosition(tick.contract_id, 10);
    }
    target_pos_manager_.OnTick(tick);
  }

  void OnOrder(const OrderResponse& order) final { target_pos_manager_.OnOrder(rsp); }

 private:
  StrategyContext* ctx_;
  MyStrategyParamManager* param_;
  ContractPtr contract_;
  TargetPositionManager target_pos_manager_;
};
```

### ConditionOrderManager

ConditionOrderManager用于支持条件单，条件单会在价格突破时触发

```cpp
#include "xyts/strategy/condition_order_manager.h"
// include other necessary header files

class MyStrategy final : public Strategy {
 public:
  explicit MyStrategy(StrategyContext* ctx)
      : ctx_(ctx),
        param_(dynamic_cast<MyStrategyParamManager*>(ctx->GetParamManager())),
        contract_(ContractTable::GetByInstrument(param_->get_instr())),
        cond_ord_manager_(ctx_) {
    ctx_->Subscribe({param_->get_instr()});

    const auto* contract = ContractTable::GetByInstrument(param_->get_instr());
    assert(contract);
    // 价格突破3950时买入5手
    ConditionOrder order{contract->contract_id, Direction::kBuy, Offset::kAuto, 5, 3950};
    cond_ord_manager_.AddConditionOrder(order);
  }
  void OnTick(const TickData& tick) final { cond_ord_manager_.OnTick(tick); }

  void OnOrder(const OrderResponse& order) final { cond_ord_manager_.OnOrder(order); }

 private:
  StrategyContext* ctx_;
  MyStrategyParamManager* param_;
  ContractPtr contract_;
  ConditionOrderManager cond_ord_manager_;
};
```

### TrailingStopManager

TrailingStopManager用于追踪止损

```cpp
#include "xyts/strategy/trailing_stop_manager.h"
// include other necessary header files

class MyStrategy final : public Strategy {
 public:
  explicit MyStrategy(StrategyContext* ctx)
      : ctx_(ctx),
        param_(dynamic_cast<MyStrategyParamManager*>(ctx->GetParamManager())),
        contract_(ContractTable::GetByInstrument(param_->get_instr())),
        trailing_stop_manager_(ctx_) {
    ctx_->Subscribe({param_->get_instr()});

    const auto* contract = ContractTable::GetByInstrument(param_->get_instr());
    assert(contract);
    // 持仓方向为多头，基准价格为3900，初始的止损价格为3880，当价格进一步上涨时，止损价格会被抬高，
    // 当最新价跌破追踪止损价格时，卖出2手来止损
    trailing_stop_manager_.AddTrailingStop(contract->contract_id, Direction::kBuy, 2, 3900, 20);
  }

  void OnTick(const TickData& tick) final { trailing_stop_manager_.OnTick(tick); }

  void OnOrder(const OrderResponse& order) final { trailing_stop_manager_.OnOrder(order); }

 private:
  StrategyContext* ctx_;
  MyStrategyParamManager* param_;
  ContractPtr contract_;
  TrailingStopManager trailing_stop_manager_;
};
```

## 算法交易

### 使用方式

```cpp
#include "xyts/strategy/algo_trading_service.h"
// #include others

class MyStrategy final : public Strategy {
 public:
  explicit MyStrategy(StrategyContext* ctx)
      : ctx_(ctx),
        param_(dynamic_cast<MyStrategyParamManager*>(ctx->GetParamManager())),
        contract_(ContractTable::GetByInstrument(param_->get_instr())),
        algo_trading_service_(ctx_) {
    ctx_->Subscribe({param_->get_instr()});
  }

  void OnTick(const TickData& tick) final {
    algo_trading_service_.OnTick(tick);
    if (satisfied condition) {
      nlohmann::json algo_params{{"algo_name", "Iceberg"},
                                 {"instr", contract_->instr},
                                 {"direction", "Buy"},
                                 {"offset", "Auto"},
                                 {"volume", 100},
                                 {"timeout", 10 * 1000000},
                                 {"price_depth", 0.5},
                                 {"interval", 2 * 1000000},
                                 {"start_time", 0},
                                 {"order_type", "price_preferred"},
                                 {"display_vol", 0.2},
                                 {"price_tick_added", 0},
                                 {"reject_interval", 3 * 1000000},
                                 {"imb_sum_ratio", 0.5},
                                 {"imb_level1_ratio", 0.5},
                                 {"traded_interval", 1 * 1000000},
                                 {"allow_pending_up_limit", false},
                                 {"allow_pending_down_limit", true}};
      algo_trading_service_.AddAlgoOrder(algo_params);
    }
  }

  void OnOrder(const OrderResponse& order) final { algo_trading_service_.OnOrder(order); }

 private:
  StrategyContext* ctx_;
  MyStrategyParamManager* param_;
  ContractPtr contract_;
  AlgoTradingService algo_trading_service_;
};
```
### 支持的算法类型：

所有参数中的时间单位均是微秒

每个算法都共有的参数

```json
{
  "algo_name": "TWAP/VWAP/...",
  "instr": "000001.XSHE",
  "direction": "Buy",
  "offset": "Auto",
  "volume": 10000,
  "timeout": "30000000"
}
```

#### TWAP

简介：根据时间加权平均市价，间隔时间发单

参数说明:

- start_time, end_time：控制TWAP开始与结束的时间（算法可能会因成交慢完成时间滞后于给定时间）
- reject_interval:订单被拒后，间隔多久重发
- duration: 每隔多少微秒发送子单
- price_tick_added: 超时后，调整子单的价格。为当前时间twap +- price_tick_added * unit_price_tick

示例

```json
{
  "algo_name": "TWAP",
  "start_time": 1705064340608000,
  "end_time": 1705064400888000,
  "duration": 30000000,
  "reject_interval": 500000,
  "price_tick_added": 1
}
```

#### VWAP

简介：根据成交量加权平均市价，间隔时间发单

具体参数解释：和TWAP参数一致，除了duration需要最小间隔分钟，如 algo_params["duration"] = 60000000

示例

```json
{
  "algo_name": "VWAP",
  "start_time": 1705064340608000,
  "end_time": 1705064400888000,
  "duration": 60000000,
  "reject_interval": 500000,
  "price_tick_added": 1
}
```

#### IS

简介：将冲击成本与时间成本考虑在内，给定风险系数下，求最优分单

具体参数解释:

- risk_aversion：风险厌恶系数(>0)，越大表示对时间冲击成本带来风险越谨慎，故会尽可能提早完成算法单(开始时，子单大小很大)
- total_time：表示执行的总时间，duration表示间隔时间，两者最小单位均需要是分钟。

示例

```json
{
  "algo_name":"IS",
  "total_time": 60000000,
  "risk_aversion": 0.5,
  "duration": 10000000,
  "reject_interval": 500000,
  "price_tick_added": 1
}
```

#### Iceberg

简介：根据显示数量参数，在某个档位或以对价发送小额单，只有上一笔子单完成，才会继续发送下笔，直到成交量满足给定额

具体参数解释：
- order_type: 发单类型，分为两种 1.时间优先"time_preferred" 2.价格优先"price_preferred"
- display_vol: 每笔子单显示数量，如果是分数(0-1)，则按当前市场五档挂单数量均值*给定分数动态计子单数量大小;如果是整数，则按给定整数定值确定每笔子单数额
- interval: 每隔多少微秒检测子单价格是否波动太大
- reject_interval: 订单被拒后,间隔多久重发
- traded_interval: 订单完全成交后,间隔多久发下笔子单
- imb_sum_ratio: 判断买卖势力加上一个比率,在买单中,如果sum_asks/sum_bids > ratio,才有可能bid1单; 如果想要更快成交(打对价)，将该ratio调为>1的值即可，反之<1
- imb_level1_ratio: 同上,但是检测的是ask1/bid1
- allow_pending_up_limit: 涨停板下，允许一直挂单(没有超时)
- allow_pending_down_limit: 跌停板下，允许一直挂单(没有超时)
- last_order_kbest: 科创板下，零股是否发市价单
- price_depth: 检查价格深度(百分比),如果子单价格超过当前价格的2 * price_depth/100，则撤销发。（其中小市值股票根据price_tick判断）
- price_tick_added: 只在价格优先下有用,会根据price_tick_added，调整价格发出

示例

```json
{
  "algo_name":"Iceberg",
  "price_depth": 0.2,
  "interval": 2000000,
  "start_time": 1705064340608000,
  "display_vol": 0.2,
  "reject_interval": 500000,
  "traded_interval": 500000,
  "imb_sum_ratio": 0,
  "imb_level1_ratio": 0,
  "price_tick_added": 0,
  "allow_pending_up_limit": true,
  "allow_pending_down_limit": true
}
```

#### Sniper

简介：市场行情达到给定条件，则立刻发对价单，数额是对价一档挂单量，直到完成给定数额

具体参数解释：

- aggressiveness: 是时间间隔，再发送上一笔子单后，超过aggressiveness微秒后，才能发送下一笔达到条件的子单

示例

```json
{
  "algo_name":"Sniper",
  "start_time": 1705064340608000,
  "aggressiveness": 100000
}
```

## BarGenerator: 实时K线合成

BarGenerator支持实时合成多种周期的K线，如3s/5s/6s/10s/15s/20s/30s/1min/3min/5min/15min等

```cpp
class MyStrategy final : public Strategy {
 public:
  explicit MyStrategy(StrategyContext* ctx)
      : ctx_(ctx),
        param_(dynamic_cast<MyStrategyParamManager*>(ctx->GetParamManager())),
        contract_(ContractTable::GetByInstrument(param_->get_instr())) {
    ctx_->Subscribe({param_->get_instr()});

    // 1min K线
    bargen_ = std::make_unique<BarGenerator>(ctx_, {contract_}, 60,
                                             [this](const BarData& bar) { OnBar1Min(bar); });
  }

  void OnTick(const TickData& tick) final { bargen_.OnTick(tick); }

  void OnBar1Min(const BarData& bar) {}

 private:
  StrategyContext* ctx_;
  MyStrategyParamManager* param_;
  ContractPtr contract_;
  std::unique_ptr<BarGenerator> bargen_;
};
```

## 合约标准命名规则

### 中国期货

`FUT_{exchange}_{symbol}-{YYYYmm}`

其中exchange和symbol对应交易所的命名，如
- FUT_SHFE_rb-202405
- FUT_CZCE_WH-202405

包括郑商所在内的所有交易所都遵循上述6位到期日的命名规则

主力合约的YYYYmm规定为111111，如
- FUT_SHFE_rb-111111
- FUT_SHFE_hc-111111

## xydata

xydata数据的组织方式

```sh
data/
    tick/
        2024-01-15/
            FUT_SHFE_rb-202405.csv
    contract/
        comm_deri_contracts_2024-01-15.csv
    holiday/
        2024.csv
```

除了holiday中的内容需要自己手动填写，合约和行情数据都能从实盘中获取并存储下来，XYTS也支持每天落地数据并更新到xydata中

### tick

快照按照日期及合约名来存储

以FUT_SHFE_rb-202405.csv前几行为例来说明数据格式

```csv
local_timestamp,exchange_timestamp,open_interest,volume,turnover,last_price,bid_volume_1,bid_price_1,ask_volume_3,ask_price_3,ask_volume_1,ask_price_1,bid_volume_4,bid_price_4,bid_volume_2,bid_price_2,ask_volume_4,ask_price_4,ask_volume_2,ask_price_2,bid_volume_5,bid_price_5,bid_volume_3,bid_price_3,ask_volume_5,ask_price_5,instrument
1705063808381581,1705055979200000,1518179,0,0,3902,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,FUT_SHFE_rb-202405
1705064340608710,1705064340500000,1520933,7550,294827500,3905,836,3905,230,3908,1036,3906,1185,3902,6,3904,283,3909,137,3907,43,3901,22,3903,477,3910,FUT_SHFE_rb-202405
1705064400607038,1705064400500000,1520956,8592,335515850,3904,13,3903,1029,3906,3,3904,1683,3900,1184,3902,160,3907,404,3905,216,3899,109,3901,280,3908,FUT_SHFE_rb-202405
1705064401083261,1705064401000000,1520861,9226,360268510,3905,10,3904,160,3907,366,3905,103,3901,124,3903,290,3908,1040,3906,1753,3900,1139,3902,305,3909,FUT_SHFE_rb-202405
1705064401638857,1705064401500000,1521087,9891,386227820,3904,231,3904,162,3907,349,3905,107,3901,5,3903,282,3908,1053,3906,1755,3900,1029,3902,437,3909,FUT_SHFE_rb-202405
```

数据字段，顺序无关紧要：

- local_timestamp: 本地收到数据的微秒时间戳
- exchange_timestamp: 交易所微秒时间戳
- open_interest: 持仓量
- volume: 成交量
- turnover: 成交额
- last_price: 最新成交价
- bid_volume_1 ~ bid_volume_5: 买1到买5的量
- bid_price_1 ~ bid_price_5: 买1到买5的价格
- ask_volume_1 ~ ask_volume_5: 卖1到卖5的量
- ask_price_1 ~ ask_price_5: 卖1到卖5的价格
- instrument: 合约标准命名


### contract

以comm_deri_contracts_2024-01-15.csv前几行为例来说明数据格式

```csv
contract_id,instr,code,exchange,product_type,contract_unit,price_tick,upper_limit_price,lower_limit_price,long_margin_rate,short_margin_rate,max_limit_order_volume,min_limit_order_volume,max_market_order_volume,min_market_order_volume,list_date,expire_date,underlying_type,underlying_symbol,exercise_date,exercise_price
1,FUT_SHFE_pb-202412,pb2412,SHFE,Futures,5,5,17290,15335,0.08,0.08,500,1,30,1,,2024-12-16,Unknown,,,0
2,FUT_SHFE_rb-202401,rb2401,SHFE,Futures,10,1,4123,3730,0.2,0.2,500,30,30,30,,2024-01-15,Unknown,,,0
3,FUT_SHFE_rb-202402,rb2402,SHFE,Futures,10,1,4008,3627,0.1,0.1,500,1,30,1,,2024-02-19,Unknown,,,0
4,FUT_SHFE_rb-202403,rb2403,SHFE,Futures,10,1,4066,3679,0.07,0.07,500,1,30,1,,2024-03-15,Unknown,,,0
5,FUT_SHFE_rb-202404,rb2404,SHFE,Futures,10,1,4085,3696,0.07,0.07,500,1,30,1,,2024-04-15,Unknown,,,0
6,FUT_SHFE_rb-202405,rb2405,SHFE,Futures,10,1,4110,3719,0.07,0.07,500,1,30,1,,2024-05-15,Unknown,,,0
```

数据字段，顺序无关紧要：

- contract_id: 从1开始连续递增，主要是为了人看的时候方便对应合约，实际上随便填也没关系
- instr: 标准合约名
- code: 合约在交易所的命名
- exchange: 交易所名
- product_type: 产品类型, Futures/Options/Stock/...
- contract_unit: 合约乘数
- price_tick: 最小价格变动单位
- upper_limit_price: 涨停价
- lower_limit_price: 跌停价
- long_margin_rate: 多头保证金率
- short_margin_rate: 空头保证金率
- max_limit_order_volume: 限价单一笔最大的量
- min_limit_order_volume: 限价单一笔最小的量
- max_market_order_volume: 市价单一笔最大的量
- min_market_order_volume: 市价单一笔最小的量
- list_date: 上市日
- expire_date: 到期日
- underlying_type: 标的的产品类型
- underlying_symbol: 标的的代码
- exercise_date: 如果是期权产品则表示行权日，否则留空
- exercise_price: 如果是期权产品则表示行权价，否则留空

### holiday

holiday中按年存储，每年一个csv，命名为YYYY.csv，里面存储的内容是当前的所有假期。

注意事项：
- 如果假期连着周末，则周末也视作法定节假日，这一做法主要是因为节假日或是节假日前一天或是连带的那个周五没有夜盘。
- 如果年底连着下一年的元旦假期，则年底的那几天算做当年的节假日

以2024.csv为例

```csv
holiday
2024-01-01
2024-02-10
2024-02-11
2024-02-12
2024-02-13
2024-02-14
2024-02-15
2024-02-16
2024-02-17
2024-04-04
2024-04-05
2024-04-06
2024-05-01
2024-05-02
2024-05-03
2024-05-04
2024-05-05
2024-06-08
2024-06-09
2024-06-10
2024-09-15
2024-09-16
2024-09-17
2024-10-01
2024-10-02
2024-10-03
2024-10-04
2024-10-05
2024-10-06
2024-10-07
```
