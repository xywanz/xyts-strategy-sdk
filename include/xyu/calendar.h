#pragma once

#include "xyu/datetime.h"

namespace xyu::calendar {

static constexpr int kDaysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

class IllegalMonthError : public std::invalid_argument {
 public:
  IllegalMonthError(int month)
      : invalid_argument("bad month number " + std::to_string(month) + "; must be 1-12") {}
};

inline bool IsLeap(int year) {
  const unsigned int uyear = static_cast<unsigned int>(year);
  return uyear % 4 == 0 && (uyear % 100 != 0 || uyear % 400 == 0);
}

// [y1, y2)
inline int LeapYears(int y1, int y2) {
  y1 -= 1;
  y2 -= 1;
  return (y2 / 4 - y1 / 4) - (y2 / 100 - y1 / 100) + (y2 / 400 - y1 / 400);
}

inline int Weekday(int year, int month, int day) {
  if (year < datetime::kMinYear || year > datetime::kMaxYear) {
    year = 2000 + year % 400;
  }
  return datetime::date(year, month, day).weekday();
}

inline int _MonthLen(int year, int month) {
  return kDaysInMonth[month] + ((month == 2 && IsLeap(year)) ? 1 : 0);
}

inline int MonthLen(int year, int month) {
  if (month < 1 || month > 12) {
    throw IllegalMonthError(month);
  }
  return _MonthLen(year, month);
}

inline std::tuple<int, int> MonthRange(int year, int month) {
  if (month < 1 || month > 12) {
    throw IllegalMonthError(month);
  }
  return {Weekday(year, month, 1), _MonthLen(year, month)};
}

inline std::tuple<int, int> _PrevMonth(int year, int month) {
  if (month == 1) {
    return {year - 1, 12};
  } else {
    return {year, month - 1};
  }
}

inline std::tuple<int, int> PrevMonth(int year, int month) {
  if (month < 1 || month > 12) {
    throw IllegalMonthError(month);
  }
  return _PrevMonth(year, month);
}

inline std::tuple<int, int> _NextMonth(int year, int month) {
  if (month == 12) {
    return {year + 1, 1};
  } else {
    return {year, month + 1};
  }
}

inline std::tuple<int, int> NextMonth(int year, int month) {
  if (month < 1 || month > 12) {
    throw IllegalMonthError(month);
  }
  return _NextMonth(year, month);
}

}  // namespace xyu::calendar
