#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "xyu/datetime.h"
#include "xyu/utility.h"

namespace xydata {

struct Session {
  Session(std::size_t _session_no, const xyu::datetime::time& open_time,
          const xyu::datetime::time& close_time, bool _is_trading_session = true);

  std::size_t session_no;
  xyu::datetime::time open;
  xyu::datetime::time close;
  std::chrono::microseconds length;
  bool is_trading_session;
};

class SessionsMap;

class Sessions {
 public:
  bool IsInTradingTime(const xyu::datetime::time& time,
                       const xyu::datetime::timedelta& delta = xyu::datetime::timedelta(0)) const;

  bool IsInTradingTime(std::chrono::microseconds ts,
                       const xyu::datetime::timedelta& delta = xyu::datetime::timedelta(0)) const;

  bool IsNearTradingDayEndTime(const xyu::datetime::time& time,
                               const xyu::datetime::timedelta& delta =
                                   xyu::datetime::timedelta(std::chrono::minutes{3})) const;

  bool IsNearTradingDayEndTime(std::chrono::microseconds ts,
                               const xyu::datetime::timedelta& delta =
                                   xyu::datetime::timedelta(std::chrono::minutes{3})) const;

  bool IsNearEndTime(const xyu::datetime::time& time,
                     const xyu::datetime::timedelta& delta =
                         xyu::datetime::timedelta(std::chrono::minutes{3})) const;

  bool IsNearEndTime(std::chrono::microseconds ts,
                     const xyu::datetime::timedelta& delta =
                         xyu::datetime::timedelta(std::chrono::minutes{3})) const;

  bool IsNearDayEndTime(const xyu::datetime::time& time,
                        const xyu::datetime::timedelta& delta =
                            xyu::datetime::timedelta(std::chrono::minutes{3})) const;

  bool IsNearDayEndTime(std::chrono::microseconds ts,
                        const xyu::datetime::timedelta& delta =
                            xyu::datetime::timedelta(std::chrono::minutes{3})) const;

  bool IsNearNightEndTime(const xyu::datetime::time& time,
                          const xyu::datetime::timedelta& delta =
                              xyu::datetime::timedelta(std::chrono::minutes{3})) const;

  bool IsNearNightEndTime(std::chrono::microseconds ts,
                          const xyu::datetime::timedelta& delta =
                              xyu::datetime::timedelta(std::chrono::minutes{3})) const;

  bool IsInBreakTime(const xyu::datetime::time& time) const;

  bool IsInBreakTime(std::chrono::microseconds ts) const;

  bool IsInNightBreakTime(const xyu::datetime::time& time) const;

  bool IsInNightBreakTime(std::chrono::microseconds ts) const;

  bool IsInDayBreakTime(const xyu::datetime::time& time) const;

  bool IsInDayBreakTime(std::chrono::microseconds ts) const;

  const std::vector<Session>& GetDayTradingSessions() const { return day_trading_sessions_; }

  const std::vector<Session>& GetNightTradingSessions() const { return night_trading_sessions_; }

  const std::vector<Session>& GetAllTradingSessions() const { return trading_sessions_; }

  const std::vector<Session>& GetNightBreakSessions() const { return night_break_sessions_; }

  const std::vector<Session>& GetDayBreakSessions() const { return day_break_sessions_; }

  const std::vector<Session>& GetAllBreakSessions() const { return break_sessions_; }

  std::optional<xyu::datetime::time> GetNightBeginTime() const {
    if (night_trading_sessions_.empty()) {
      return std::nullopt;
    } else {
      return night_trading_sessions_.front().open;
    }
  }

  std::optional<xyu::datetime::time> GetNightEndTime() const {
    if (night_trading_sessions_.empty()) {
      return std::nullopt;
    } else {
      return night_trading_sessions_.back().close;
    }
  }

  std::optional<xyu::datetime::time> GetDayBeginTime() const {
    if (day_trading_sessions_.empty()) {
      return std::nullopt;
    } else {
      return day_trading_sessions_.front().open;
    }
  }

  std::optional<xyu::datetime::time> GetDayEndTime() const {
    if (day_trading_sessions_.empty()) {
      return std::nullopt;
    } else {
      return day_trading_sessions_.back().close;
    }
  }

  bool HasNightSessions() const { return !night_trading_sessions_.empty(); }

  bool HasDaySessions() const { return !day_trading_sessions_.empty(); }

  std::chrono::microseconds TotalTradingSessionLength() const {
    return total_trading_session_length_;
  }

  std::chrono::microseconds NightTradingSessionLength() const {
    return night_trading_session_length_;
  }

  std::chrono::microseconds DayTradingSessionLength() const { return day_trading_session_length_; }

  std::chrono::microseconds TotalBreakSessionLength() const { return total_break_session_length_; }

  std::chrono::microseconds NightBreakSessionLength() const { return night_break_session_length_; }

  std::chrono::microseconds DayBreakSessionLength() const { return day_break_session_length_; }

 private:
  Sessions(const std::vector<Session>& night_sessions, const std::vector<Session>& day_sessions);

  friend SessionsMap;
  std::vector<Session> trading_sessions_;
  std::vector<Session> night_trading_sessions_;
  std::vector<Session> day_trading_sessions_;
  std::chrono::microseconds total_trading_session_length_{0};
  std::chrono::microseconds night_trading_session_length_{0};
  std::chrono::microseconds day_trading_session_length_{0};

  std::vector<Session> break_sessions_;
  std::vector<Session> day_break_sessions_;
  std::vector<Session> night_break_sessions_;
  std::chrono::microseconds total_break_session_length_{0};
  std::chrono::microseconds day_break_session_length_{0};
  std::chrono::microseconds night_break_session_length_{0};
};

using SessionsPtr = const Sessions*;

class SessionsMap : public xyu::NonCopyableNonMoveable {
 public:
  static SessionsMap& Instance() {
    static SessionsMap smap;
    return smap;
  }

  SessionsPtr GetSessions(const std::string& sym);

 private:
  SessionsMap();

  std::map<std::string, Sessions> map_;
};

inline SessionsPtr GetSessions(const std::string& sym) {
  return SessionsMap::Instance().GetSessions(sym);
}

}  // namespace xydata
