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
inline bool IsPositionOpen(PositionEffect position_effect) {
  return position_effect == PositionEffect::kOpen;
}

// 是否是平仓，平仓包括平仓、平今、平昨
inline bool IsPositionClose(PositionEffect position_effect) {
  return static_cast<uint8_t>(position_effect) &
         (static_cast<uint8_t>(PositionEffect::kClose) |
          static_cast<uint8_t>(PositionEffect::kCloseToday) |
          static_cast<uint8_t>(PositionEffect::kCloseYesterday));
}

inline bool IsValidPositionEffect(PositionEffect position_effect) {
  return static_cast<uint8_t>(position_effect) &
         (static_cast<uint8_t>(PositionEffect::kOpen) |
          static_cast<uint8_t>(PositionEffect::kClose) |
          static_cast<uint8_t>(PositionEffect::kCloseToday) |
          static_cast<uint8_t>(PositionEffect::kCloseYesterday) |
          static_cast<uint8_t>(PositionEffect::kAuto) |
          static_cast<uint8_t>(PositionEffect::kSmart) |
          static_cast<uint8_t>(PositionEffect::kInternalTrade));
}

inline bool IsValidRealPositionEffect(PositionEffect position_effect) {
  return static_cast<uint8_t>(position_effect) &
         (static_cast<uint8_t>(PositionEffect::kOpen) |
          static_cast<uint8_t>(PositionEffect::kClose) |
          static_cast<uint8_t>(PositionEffect::kCloseToday) |
          static_cast<uint8_t>(PositionEffect::kCloseYesterday));
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

inline std::string ToString(PositionEffect position_effect) {
  switch (position_effect) {
    case PositionEffect::kOpen: {
      return "Open";
    }
    case PositionEffect::kClose: {
      return "Close";
    }
    case PositionEffect::kCloseToday: {
      return "CloseToday";
    }
    case PositionEffect::kCloseYesterday: {
      return "CloseYesterday";
    }
    case PositionEffect::kAuto: {
      return "Auto";
    }
    case PositionEffect::kSmart: {
      return "Smart";
    }
    case PositionEffect::kInternalTrade: {
      return "InternalTrade";
    }
    default: {
      return "Unknown";
    }
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
    case OrderType::kBBO: {
      return "BBO";
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

inline Direction ParseDirectionFromString(const std::string& str) {
  if (str == "Buy") {
    return Direction::kBuy;
  } else if (str == "Sell") {
    return Direction::kSell;
  } else {
    return Direction::kUnknown;
  }
}

inline PositionEffect ParsePositionEffectFromString(const std::string& str) {
  if (str == "Open") {
    return PositionEffect::kOpen;
  } else if (str == "Close") {
    return PositionEffect::kClose;
  } else if (str == "CloseToday") {
    return PositionEffect::kCloseToday;
  } else if (str == "CloseYesterday") {
    return PositionEffect::kCloseYesterday;
  } else if (str == "Auto") {
    return PositionEffect::kAuto;
  } else if (str == "Smart") {
    return PositionEffect::kSmart;
  } else if (str == "InternalTrade") {
    return PositionEffect::kInternalTrade;
  } else {
    return PositionEffect::kUnknown;
  }
}

inline OrderType ParseOrderTypeFromString(const std::string& str) {
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
  } else if (str == "BBO") {
    return OrderType::kBBO;
  } else {
    return OrderType::kUnknown;
  }
}

inline OrderStatus ParseOrderStatusFromString(const std::string& str) {
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

inline ProductType ParseProductTypeFromString(const std::string& str) {
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

inline ExchangeId ParseExchangeIdFromString(const std::string& str) {
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
