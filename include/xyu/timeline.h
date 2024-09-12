#pragma once

#include <cstdint>
#include <functional>
#include <map>

namespace xyu {

class Timeline {
 public:
  using Callback = std::function<void(int64_t)>;

  int64_t AddEvent(int64_t timeout, Callback&& cb, bool repeat = false);

  void RemoveEvent(int64_t id);

  void GoForward(int64_t t);

  void Reset();

  int64_t now() const { return now_; }

 private:
  struct Event {
    int64_t id;
    int64_t interval;
    Callback cb;
  };

  int64_t now_ = 0;
  int64_t next_id_ = 0;

  using EventMap = std::multimap<int64_t, Event>;
  using EventIter = EventMap::iterator;
  std::multimap<int64_t, Event> timeouts_;
  std::map<int64_t, EventIter> event_id_map_;
};

}  // namespace xyu
