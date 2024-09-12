#pragma once

#include <cstdint>

namespace xyts {

/*
 * 错误码分为几个阶段的错误码：
 * 1. 发送前
 * 2. 发送错误
 * 3. 发送后
 */
enum class ErrorCode : uint8_t {
  kNoError = 0,

  kInvalidOrder,                // 无效的订单
  kOrderTimeLimit,              // 在允许的下单时间之外下单
  kInvalidPrice,                // 无效的价格
  kExceedMaxOrderLimit,         // 当日报单数超限
  kExceedMaxCancelNumLimit,     // 当日撤单数超限
  kInvalidVolume,               // 无效的报单手数
  kSelfTrade,                   // 自成交
  kPositionExceedLimit,         // 持仓数量超过限制
  kLogicalPositionExceedLimit,  // 逻辑持仓超过限制
  kInsufficientPosition,        // 仓位不足
  kInsufficientFund,            // 资金不足
  kExceedThrottleRateLimit,     // 订单流速超限
  kExceedFillPosRatioLimit,     // 成交持仓比超限
  kOrderDisabled,               // 禁止下单
  kCancellationDisabled,        // 禁止撤单
  kCircuitBreakerRisk,          // 发单价格有熔断风险
  kExpensiveFeeRisk,            // 可能导致高额手续费
  kNoTradableAccount,

  kSendFailed = 128,  // 调用API报单失败
  kPacketLost,        // 丢包

  kRejected,  // 被拒单
};

#define ERROR_CODE_CASE(name) \
  case ErrorCode::k##name: {  \
    return #name;             \
  }

inline const char* ToString(ErrorCode error_code) {
  switch (error_code) {
    ERROR_CODE_CASE(NoError);
    ERROR_CODE_CASE(InvalidOrder);
    ERROR_CODE_CASE(OrderTimeLimit);
    ERROR_CODE_CASE(InvalidPrice);
    ERROR_CODE_CASE(ExceedMaxOrderLimit);
    ERROR_CODE_CASE(ExceedMaxCancelNumLimit);
    ERROR_CODE_CASE(InvalidVolume);
    ERROR_CODE_CASE(SelfTrade);
    ERROR_CODE_CASE(PositionExceedLimit);
    ERROR_CODE_CASE(LogicalPositionExceedLimit);
    ERROR_CODE_CASE(InsufficientPosition);
    ERROR_CODE_CASE(InsufficientFund);
    ERROR_CODE_CASE(ExceedThrottleRateLimit);
    ERROR_CODE_CASE(ExceedFillPosRatioLimit);
    ERROR_CODE_CASE(OrderDisabled);
    ERROR_CODE_CASE(CancellationDisabled);
    ERROR_CODE_CASE(CircuitBreakerRisk);
    ERROR_CODE_CASE(ExpensiveFeeRisk);
    ERROR_CODE_CASE(NoTradableAccount);
    ERROR_CODE_CASE(SendFailed);
    ERROR_CODE_CASE(PacketLost);
    ERROR_CODE_CASE(Rejected);
    default: {
      return "UnknownError";
    }
  }
}

#undef ERROR_CODE_CASE

}  // namespace xyts
