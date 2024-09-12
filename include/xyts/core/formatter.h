#pragma once

#include "fmt/format.h"
#include "xyts/utils/trade_utility.h"

namespace fmt {

#define DEFINE_BASE_TYPE_FORMATTER(type)                                                           \
  template <>                                                                                      \
  struct formatter<::xyts::type> {                                                                 \
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.end(); } \
    template <typename FormatContext>                                                              \
    auto format(const ::xyts::type& obj, FormatContext& ctx) -> decltype(ctx.out()) {              \
      return ::fmt::format_to(ctx.out(), "{}", ::xyts::ToString(obj));                             \
    }                                                                                              \
  };

DEFINE_BASE_TYPE_FORMATTER(OrderStatus);
DEFINE_BASE_TYPE_FORMATTER(Direction);
DEFINE_BASE_TYPE_FORMATTER(PositionEffect);
DEFINE_BASE_TYPE_FORMATTER(OrderType);
DEFINE_BASE_TYPE_FORMATTER(ProductType);
DEFINE_BASE_TYPE_FORMATTER(ErrorCode);

}  // namespace fmt
