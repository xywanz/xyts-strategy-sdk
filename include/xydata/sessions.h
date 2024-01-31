#pragma once

#include <map>
#include <string>
#include <vector>

#include "xyu/datetime.h"
#include "xyu/utility.h"

namespace xydata {

struct Session {
  xyu::datetime::time open;
  xyu::datetime::time close;
};

class SessionsMap;

class Sessions {
 public:
  const std::vector<Session>& Get() const { return sessions_; }

  bool Contains(const xyu::datetime::time& time) const;

 private:
  explicit Sessions(std::vector<Session>&& sessions) : sessions_(std::move(sessions)) {}

  friend SessionsMap;
  std::vector<Session> sessions_;
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
