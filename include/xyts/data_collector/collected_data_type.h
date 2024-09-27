#pragma once

namespace xyts {

enum class CollectedDataType {
  kNone = 0,       //
  kAlarm,          // 企业微信消息通知
  kTrader,         // trader相关
  kStrategy,       // strategy相关
  kProcessStatus,  // 进程状态
  kDataCenter,     // 行情源
  kAlgoTrading,    // 算法单相关
  kDataTypeNum,
};

}
