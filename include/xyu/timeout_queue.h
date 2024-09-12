#pragma once

#include <cstdint>
#include <functional>
#include <map>

namespace xyu {

class TimeoutQueue {
 public:
  using EventId = int64_t;
  using Callback = std::function<void(EventId, int64_t)>;

  EventId Add(int64_t now, int64_t timeout, Callback cb);

  EventId AddPeriodic(int64_t now, int64_t interval, Callback cb);

  void Remove(EventId id);

  std::size_t RunOnce(int64_t now);

  std::size_t RunLoop(int64_t now);

  int64_t NextExpiration() const;

  void Clear() {
    event_id_map_.clear();
    timeouts_.clear();
  }

  std::size_t size() const { return timeouts_.size(); }

 private:
  struct Event {
    EventId id;
    int64_t interval;
    Callback cb;
  };

  using EventMap = std::multimap<int64_t, Event>;
  using EventIter = EventMap::iterator;
  EventMap timeouts_;
  std::map<EventId, EventIter> event_id_map_;
  EventId next_id_ = 1;
};

}  // namespace xyu