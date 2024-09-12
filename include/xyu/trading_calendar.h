#pragma once

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "xyu/datetime.h"

namespace xyu {

/**
 * @brief 超出了交易日历范围
 * 查询过程中所涉及到的日期大于交易日历中的最大日期，或是小于交易日历中的最小日期
 */
class OutOfTradingCalendar : public std::out_of_range {
 public:
  using out_of_range::out_of_range;
};

/**
 * @brief 交易日历没有成功初始化
 * 如果交易日历没有成功初始化就使用会抛出该错误
 */
class BadTradingCalendar : public std::runtime_error {
 public:
  using runtime_error::runtime_error;
};

/**
 * @brief 通用交易日历
 * TradingCalendar只是从已有的交易日历文件中加载交易日数据，提供相应的utils函数，并不能自动地生成交易日历。
 */
class TradingCalendar {
 public:
  /**
   * @brief 从csv文件加载交易日历
   * 每个Load函数都可以重复调用，每次调用都会用新的交易日历替换掉原本的。
   * csv文件应该具有如下格式：
   *    TradingDay
   *    2022-07-04
   *    2022-07-05
   *    ...
   * 第一行必须为header，且必须包含TradingDay列，也可以有其他列，但其他列目前还不会被使用。
   * 其余行的TradingDay必须为iso格式的，即YYYY-MM-DD，可以重复，也不要求顺序。如果没有数据
   * 会视为加载失败。
   * @param calendar_file 交易日历文件路径
   */
  explicit TradingCalendar(std::string_view calendar_file);

  /**
   * @brief 加载交易日的date数组
   * date数组中的所有date都被视为交易日，可以重复，也不要求顺序。
   * @param trading_dates 交易日数组
   */
  explicit TradingCalendar(const std::vector<xyu::datetime::date>& trading_dates);

  /**
   * @brief 加载交易日的date数组
   * date数组中的所有date都被视为交易日，可以重复，也不要求顺序。
   * @param trading_date_strings iso格式的交易日字符串数组
   */
  explicit TradingCalendar(const std::vector<std::string>& trading_date_strings);

  /**
   * @brief 某日是否为交易日
   *
   * @param date xyu::datetime::date类型的日期
   * @return true 该日是交易日
   * @return false 该日不是交易日
   * @throw OutOfTradingCalendar date在交易日历范围外，无法判断
   */
  bool IsTradingDay(const xyu::datetime::date& date) const;

  /**
   * @brief 某日是否为交易日
   *
   * @param date_string iso格式的日期字符串
   * @return true 该日是交易日
   * @return false 该日不是交易日
   * @throw OutOfTradingCalendar date在交易日历范围外，无法判断
   */
  bool IsTradingDay(const std::string& date_string) const {
    return IsTradingDay(xyu::datetime::date::fromisoformat(date_string));
  }

  /**
   * @brief 某日的下一个交易日
   * 如果date是交易日，返回的是下一个交易日。
   * 如果date不是交易日，返回的是接来下的第一个交易日。
   * @param date xyu::datetime::date类型的日期
   * @return xyu::datetime::date 交易日
   * @throw OutOfTradingCalendar date或是查询结果在交易日历范围外，无法获取
   */
  xyu::datetime::date NextTradingDay(const xyu::datetime::date& date) const;

  /**
   * @brief 某日的下一个交易日
   * 如果date是交易日，返回的是下一个交易日。
   * 如果date不是交易日，返回的是接来下的第一个交易日。
   * @param date_string iso格式的日期字符串
   * @return xyu::datetime::date 交易日
   * @throw OutOfTradingCalendar date或是查询结果在交易日历范围外，无法获取
   */
  xyu::datetime::date NextTradingDay(const std::string& date_string) const {
    return NextTradingDay(xyu::datetime::date::fromisoformat(date_string));
  }

  /**
   * @brief 某日的前一个交易日
   * 如果date是交易日，返回的是前一个交易日。
   * 如果date不是交易日，返回的是往前的第一个交易日。
   * @param date xyu::datetime::date类型的日期
   * @return xyu::datetime::date 交易日
   * @throw OutOfTradingCalendar date或是查询结果在交易日历范围外，无法获取
   */
  xyu::datetime::date PrevTradingDay(const xyu::datetime::date& date) const;

  /**
   * @brief 某日的前一个交易日
   * 如果date是交易日，返回的是前一个交易日。
   * 如果date不是交易日，返回的是往前的第一个交易日。
   * @param date_string iso格式的日期字符串
   * @return xyu::datetime::date 交易日
   * @throw OutOfTradingCalendar date或是查询结果在交易日历范围外，无法获取
   */
  xyu::datetime::date PrevTradingDay(const std::string& date_string) const {
    return PrevTradingDay(xyu::datetime::date::fromisoformat(date_string));
  }

  /**
   * @brief 某日往前或往后数的第n个交易日
   * @param date 日期
   * @param offset 偏移，小于0表示往前，大于0表示往后，等于0则直接返回date
   * @return xyu::datetime::date 交易日
   * @throw OutOfTradingCalendar date或是查询结果在交易日历范围外，无法获取
   */
  xyu::datetime::date TradingDayShift(const xyu::datetime::date& date, std::ptrdiff_t offset) const;

  /**
   * @brief 某日往前或往后数的第n个交易日
   * @param date_string iso格式的日期字符串
   * @param offset 偏移，小于0表示往前，大于0表示往后，等于0则直接返回date
   * @return xyu::datetime::date 交易日
   * @throw OutOfTradingCalendar date或是查询结果在交易日历范围外，无法获取
   */
  xyu::datetime::date TradingDayShift(const std::string& date_string, std::ptrdiff_t offset) const {
    return TradingDayShift(xyu::datetime::date::fromisoformat(date_string), offset);
  }

  /**
   * @brief 返回两个日期之间的交易日数组
   * 返回[begin_date, end_date]之间的交易日数组
   * @param begin_date 起始日期
   * @param end_date 结束日期
   * @return std::vector<xyu::datetime::date> 交易日数组
   * @throw OutOfTradingCalendar begin_date或是end_date或是查询结果在交易日历范围外，无法获取
   */
  std::vector<xyu::datetime::date> TradingDaysBetween(const xyu::datetime::date& begin_date,
                                                      const xyu::datetime::date& end_date) const;

  /**
   * @brief 返回两个日期之间的交易日数组
   * 返回[begin_date, end_date]之间的交易日数组
   * @param begin_date_string iso格式的起始日期字符串
   * @param end_date_string iso格式的结束日期字符串
   * @return std::vector<xyu::datetime::date> 交易日数组
   * @throw OutOfTradingCalendar begin_date或是end_date或是查询结果在交易日历范围外，无法获取
   */
  std::vector<xyu::datetime::date> TradingDaysBetween(const std::string& begin_date_string,
                                                      const std::string& end_date_string) const {
    return TradingDaysBetween(xyu::datetime::date::fromisoformat(begin_date_string),
                              xyu::datetime::date::fromisoformat(end_date_string));
  }

  /**
   * @brief 返回两个日期之间的交易日天数
   * 返回[begin_date, end_date]之间的交易日天数
   * @param begin_date 起始日期
   * @param end_date 结束日期
   * @return std::size_t 交易日数组
   * @throw OutOfTradingCalendar begin_date或是end_date或是查询过程在交易日历范围外，无法获取
   */
  std::size_t CountTradingDaysBetween(const xyu::datetime::date& begin_date,
                                      const xyu::datetime::date& end_date) const;

  /**
   * @brief 返回两个日期之间的交易日天数
   * 返回[begin_date, end_date]之间的交易日天数
   * @param begin_date_string iso格式的起始日期字符串
   * @param end_date_string iso格式的结束日期字符串
   * @return std::size_t 交易日数组
   * @throw OutOfTradingCalendar begin_date或是end_date或是查询过程在交易日历范围外，无法获取
   */
  std::size_t CountTradingDaysBetween(const std::string& begin_date_string,
                                      const std::string& end_date_string) const {
    return CountTradingDaysBetween(xyu::datetime::date::fromisoformat(begin_date_string),
                                   xyu::datetime::date::fromisoformat(end_date_string));
  }

 private:
  std::set<xyu::datetime::date> trading_dates_;
};

}  // namespace xyu
