#pragma once

#include <cmath>
#include <cstdint>
#include <deque>
#include <set>

namespace xyts::strategy {

class RollingAccumulator {
 public:
  explicit RollingAccumulator(std::size_t window_size) : window_size_(window_size) {}

  void Update(double val);

  void Clear() {
    window_.clear();
    sum_ = 0;
  }

  double sum() const { return sum_; }

  bool full() const { return window_.size() == window_size_; }

  std::size_t size() const { return window_.size(); }

 private:
  const std::size_t window_size_;
  std::deque<double> window_;
  double sum_ = 0;
};

class RollingMean {
 public:
  explicit RollingMean(std::size_t window_size) : window_size_(window_size) {}

  void Update(double val);

  double mean() const { return mean_; }

  bool full() const { return window_.size() == window_size_; }

  std::size_t size() const { return window_.size(); }

 private:
  const std::size_t window_size_;
  std::deque<double> window_;
  double mean_ = 0;
};

class RollingMeanVar {
 public:
  explicit RollingMeanVar(std::size_t window_size) : window_size_(window_size) {}

  void Update(double val);

  double mean() const { return mean_; }

  double var() const { return s_ / (window_.size() - 1); }

  double std() const {
    double v = var();
    if (v > 0)
      return std::sqrt(v);
    else
      return 0;
  }

  double zscore(double val) const {
    double _std = std();
    return _std > 0 ? (val - mean()) / _std : 0;
  }

  double last_zscore() const { return zscore(window_.front()); }

  bool full() const { return window_.size() == window_size_; }

  std::size_t size() const { return window_.size(); }

 private:
  const std::size_t window_size_;
  std::deque<double> window_;
  double mean_ = 0;
  double s_ = 0;
};

class RollingQuantile {
 public:
  explicit RollingQuantile(std::size_t window_size) : window_size_(window_size) {}

  void Update(double val);

  double median() const;

  double quartile(double q) const;

  double iqr() const { return (quartile(0.75) - quartile(0.25)); }

  double min() const { return *min_heap_.begin(); }

  double max() const { return *(--max_heap_.end()); }

  double zscore(double val) const {
    double _iqr = iqr();
    return _iqr > 0 ? (val - median()) / _iqr : 0;
  }

  double last_zscore() const { return zscore(window_.front()); }

  bool full() const { return window_.size() == window_size_; }

  std::size_t size() const { return window_.size(); }

 private:
  const std::size_t window_size_;
  std::deque<double> window_;
  std::multiset<double> min_heap_;
  std::multiset<double> max_heap_;
};

class EMA {
 public:
  explicit EMA(std::size_t window_size) : alpha_(2.0 / (window_size + 1.0)) {}

  void Update(double val);

  double ema() const { return ema_; }

 private:
  double alpha_;
  double ema_ = 0;
};

}  // namespace xyts::strategy
