#pragma once

#include <cstring>
#include <string>

#include "xyts/core/error_code.h"
#include "xyts/core/trade_msg.h"

namespace xyts {

// 对手方，只针对买卖方向有效
inline Direction OppositeDirection(Direction direction) {
  return direction == Direction::kBuy ? Direction::kSell : Direction::kBuy;
}

inline bool IsValidDirection(Direction direction) {
  return direction == Direction::kBuy || direction == Direction::kSell;
}

// 是否是开仓
inline bool IsOffsetOpen(Offset offset) { return offset == Offset::kOpen; }

// 是否是平仓，平仓包括平仓、平今、平昨
inline bool IsOffsetClose(Offset offset) {
  return static_cast<uint8_t>(offset) &
         (static_cast<uint8_t>(Offset::kClose) | static_cast<uint8_t>(Offset::kCloseToday) |
          static_cast<uint8_t>(Offset::kCloseYesterday));
}

inline bool IsValidOffset(Offset offset) {
  return static_cast<uint8_t>(offset) &
         (static_cast<uint8_t>(Offset::kOpen) | static_cast<uint8_t>(Offset::kClose) |
          static_cast<uint8_t>(Offset::kCloseToday) |
          static_cast<uint8_t>(Offset::kCloseYesterday) | static_cast<uint8_t>(Offset::kAuto) |
          static_cast<uint8_t>(Offset::kInternalTrade));
}

inline bool IsValidOrderType(OrderType type) {
  return static_cast<uint8_t>(type) < static_cast<uint8_t>(OrderType::kOrderTypeNum);
}

inline std::string ToString(Direction direction) {
  switch (direction) {
    case Direction::kBuy:
      return "Buy";
    case Direction::kSell:
      return "Sell";
    default:
      return "Unknown";
  }
}

inline std::string ToString(Offset offset) {
  switch (offset) {
    case Offset::kOpen:
      return "Open";
    case Offset::kClose:
      return "Close";
    case Offset::kCloseToday:
      return "CloseToday";
    case Offset::kCloseYesterday:
      return "CloseYesterday";
    case Offset::kInternalTrade:
      return "InternalTrade";
    case Offset::kAuto:
      return "Auto";
    default:
      return "Unknown";
  }
}

inline std::string ToString(OrderType order_type) {
  switch (order_type) {
    case OrderType::kLimit: {
      return "Limit";
    }
    case OrderType::kMarket: {
      return "Market";
    }
    case OrderType::kFAK: {
      return "FAK";
    }
    case OrderType::kFOK: {
      return "FOK";
    }
    case OrderType::kMakerOnly: {
      return "MakerOnly";
    }
    default: {
      return "Unknown";
    }
  }
}

inline std::string ToString(OrderStatus status) {
  switch (status) {
    case OrderStatus::kSubmitting:
      return "Submitting";
    case OrderStatus::kRejected:
      return "Rejected";
    case OrderStatus::kAccepted:
      return "Accepted";
    case OrderStatus::kPartTraded:
      return "PartTraded";
    case OrderStatus::kAllTraded:
      return "AllTraded";
    case OrderStatus::kCanceled:
      return "Canceled";
    default:
      return "Unknown";
  }
}

inline std::string ToString(ProductType type) {
  switch (type) {
    case ProductType::kFutures:
      return "Futures";
    case ProductType::kOptions:
      return "Options";
    case ProductType::kStock:
      return "Stock";
    case ProductType::kETF:
      return "ETF";
    case ProductType::kIndex:
      return "Index";
    case ProductType::kBond:
      return "Bond";
    case ProductType::kFund:
      return "Fund";
    case ProductType::kSpot:
      return "Spot";
    default:
      return "Unknown";
  }
}

inline Direction Str2Direction(const std::string& str) {
  if (str == "Buy") {
    return Direction::kBuy;
  } else if (str == "Sell") {
    return Direction::kSell;
  } else {
    return Direction::kUnknown;
  }
}

inline Offset Str2Offset(const std::string& str) {
  if (str == "Open") {
    return Offset::kOpen;
  } else if (str == "Close") {
    return Offset::kClose;
  } else if (str == "CloseToday") {
    return Offset::kCloseToday;
  } else if (str == "CloseYesterday") {
    return Offset::kCloseYesterday;
  } else if (str == "InternalTrade") {
    return Offset::kInternalTrade;
  } else if (str == "Auto") {
    return Offset::kAuto;
  } else {
    return Offset::kUnknown;
  }
}

inline OrderType Str2OrderType(const std::string& str) {
  if (str == "Limit") {
    return OrderType::kLimit;
  } else if (str == "Market") {
    return OrderType::kMarket;
  } else if (str == "FAK") {
    return OrderType::kFAK;
  } else if (str == "FOK") {
    return OrderType::kFOK;
  } else if (str == "MakerOnly") {
    return OrderType::kMakerOnly;
  } else {
    return OrderType::kUnknown;
  }
}

inline OrderStatus Str2OrderStatus(const std::string& str) {
  if (str == "Submitting") {
    return OrderStatus::kSubmitting;
  } else if (str == "Rejected") {
    return OrderStatus::kRejected;
  } else if (str == "Accepted") {
    return OrderStatus::kAccepted;
  } else if (str == "PartTraded") {
    return OrderStatus::kPartTraded;
  } else if (str == "AllTraded") {
    return OrderStatus::kAllTraded;
  } else if (str == "Canceled") {
    return OrderStatus::kCanceled;
  } else {
    return OrderStatus::kUnknown;
  }
}

inline ProductType Str2ProductType(const std::string& str) {
  if (strncasecmp(str.c_str(), "Futures", str.size()) == 0) {
    return ProductType::kFutures;
  } else if (strncasecmp(str.c_str(), "Options", str.size()) == 0) {
    return ProductType::kOptions;
  } else if (strncasecmp(str.c_str(), "Stock", str.size()) == 0) {
    return ProductType::kStock;
  } else if (strncasecmp(str.c_str(), "ETF", str.size()) == 0) {
    return ProductType::kETF;
  } else if (strncasecmp(str.c_str(), "Index", str.size()) == 0) {
    return ProductType::kIndex;
  } else if (strncasecmp(str.c_str(), "Bond", str.size()) == 0) {
    return ProductType::kBond;
  } else if (strncasecmp(str.c_str(), "Fund", str.size()) == 0) {
    return ProductType::kFund;
  } else if (strncasecmp(str.c_str(), "Spot", str.size()) == 0) {
    return ProductType::kSpot;
  } else {
    return ProductType::kUnknown;
  }
}

inline ExchangeId Str2ExchangeId(const std::string& str) {
  if (str == exchange::kSHFE) {
    return ExchangeId::kSHFE;
  } else if (str == exchange::kINE) {
    return ExchangeId::kINE;
  } else if (str == exchange::kCFFEX) {
    return ExchangeId::kCFFEX;
  } else if (str == exchange::kCZCE) {
    return ExchangeId::kCZCE;
  } else if (str == exchange::kSSE) {
    return ExchangeId::kSSE;
  } else if (str == exchange::kSZE) {
    return ExchangeId::kSZE;
  } else if (str == exchange::kGFEX) {
    return ExchangeId::kGFEX;
  } else {
    return ExchangeId::kUnknown;
  }
}

inline bool CheckProtocolVersion(int version) {
  return version >= kMinRequiredProtocolVersion && version <= kMaxCompatibleProtocolVersion;
}

}  // namespace xyts
