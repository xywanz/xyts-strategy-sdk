#pragma once

#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "xyu/datetime.h"

namespace xydata {

class BadHolidayFile : public std::runtime_error {
 public:
  explicit BadHolidayFile(const std::string& path) : runtime_error("Bad holiday file: " + path) {}
};

class TradingCalendar {
 public:
  explicit TradingCalendar(const std::filesystem::path& holiday_dir);

  bool IsTradingDay(const xyu::datetime::date& date);

  bool IsTradingDay(const std::string& date_string) {
    return IsTradingDay(xyu::datetime::date::fromisoformat(date_string));
  }

  bool IsHoliday(const xyu::datetime::date& date);

  bool IsHoliday(const std::string& date_string) {
    return IsHoliday(xyu::datetime::date::fromisoformat(date_string));
  }

  xyu::datetime::date NextTradingDay(const xyu::datetime::date& date);

  xyu::datetime::date NextTradingDay(const std::string& date_string) {
    return NextTradingDay(xyu::datetime::date::fromisoformat(date_string));
  }

  xyu::datetime::date PrevTradingDay(const xyu::datetime::date& date);

  xyu::datetime::date PrevTradingDay(const std::string& date_string) {
    return PrevTradingDay(xyu::datetime::date::fromisoformat(date_string));
  }

  xyu::datetime::date TradingDayShift(const xyu::datetime::date& date, std::ptrdiff_t offset);

  xyu::datetime::date TradingDayShift(const std::string& date_string, std::ptrdiff_t offset) {
    return TradingDayShift(xyu::datetime::date::fromisoformat(date_string), offset);
  }

  std::vector<xyu::datetime::date> TradingDaysBetween(const xyu::datetime::date& begin_date,
                                                      const xyu::datetime::date& end_date);

  std::vector<xyu::datetime::date> TradingDaysBetween(const std::string& begin_date_string,
                                                      const std::string& end_date_string) {
    return TradingDaysBetween(xyu::datetime::date::fromisoformat(begin_date_string),
                              xyu::datetime::date::fromisoformat(end_date_string));
  }

  std::size_t CountTradingDaysBetween(const xyu::datetime::date& begin_date,
                                      const xyu::datetime::date& end_date);

  std::size_t CountTradingDaysBetween(const std::string& begin_date_string,
                                      const std::string& end_date_string) {
    return CountTradingDaysBetween(xyu::datetime::date::fromisoformat(begin_date_string),
                                   xyu::datetime::date::fromisoformat(end_date_string));
  }

 private:
  void LoadHolidays(int year);

  std::filesystem::path holiday_dir_;
  std::map<int, std::set<xyu::datetime::date>> holidays_;
};

}  // namespace xydata
