#pragma once

#include "fmt/format.h"
#include "xyts/core/log.h"
#include "xyu/datetime.h"

MARK_AS_COPY_LOGGABLE(xyu::datetime::timedelta);
MARK_AS_COPY_LOGGABLE(xyu::datetime::date);
MARK_AS_COPY_LOGGABLE(xyu::datetime::time);
MARK_AS_COPY_LOGGABLE(xyu::datetime::datetime);

namespace fmt {
#define DEFINE_DATETIME_FORMATTER(type)                                                            \
  template <>                                                                                      \
  struct formatter<::xyu::datetime::type> {                                                        \
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.end(); } \
    template <typename FormatContext>                                                              \
    auto format(const ::xyu::datetime::type& obj, FormatContext& ctx) -> decltype(ctx.out()) {     \
      return ::fmt::format_to(ctx.out(), "{}", obj.str());                                         \
    }                                                                                              \
  };

DEFINE_DATETIME_FORMATTER(timedelta);
DEFINE_DATETIME_FORMATTER(date);
DEFINE_DATETIME_FORMATTER(time);
DEFINE_DATETIME_FORMATTER(datetime);
}  // namespace fmt
