#pragma once

#include <chrono>
#include <memory>
#include <type_traits>

#include "quill/Quill.h"
#include "xyts/base/formatter.h"

namespace xyts {

class XyLogger {
 public:
  explicit XyLogger(quill::Logger* logger) : logger_(logger) {}

  quill::Logger* logger() { return logger_; }
  std::string loglevel() const;

 private:
  quill::Logger* logger_ = nullptr;
};

class XyLog {
 private:
  XyLog(const XyLog&) = delete;
  XyLog& operator=(const XyLog&) = delete;

  XyLog();

 public:
  ~XyLog();
  static XyLog& Instance() {
    static XyLog log;
    return log;
  }
  /*
  logfile 日志文件名  logfile_2021-07-19.log
  loglevel 日志等级 trace debug info warn error fatal
  max_files 按天存储，最多保留多少天
  */
  void Init(const std::string& logfile, const std::string& loglevel = "info");
  void Flush();

  // dynamically set log level. thread safe
  bool set_log_level(const std::string& loglevel);

  std::string log_level() const;

  XyLogger* logger() { return logger_.get(); }

 private:
  std::unique_ptr<XyLogger> logger_ = nullptr;
};

#define XY_LOG_INSTANCE ::xyts::XyLog::Instance()
#define XY_LOGGER XY_LOG_INSTANCE.logger()
#define XY_LOGGER_FLUSH XY_LOG_INSTANCE.Flush()

inline void InitLogger(const std::string& logfile, const std::string& loglevel = "info") {
  XY_LOG_INSTANCE.Init(logfile, loglevel);
}

std::shared_ptr<XyLogger> GetCustomLogger(const std::string& logfile);

#define LOGGER_LOG_TRACE_L3(_logger, args...) QUILL_LOG_TRACE_L3(_logger->logger(), args)
#define LOGGER_LOG_TRACE_L2(_logger, args...) QUILL_LOG_TRACE_L2(_logger->logger(), args)
#define LOGGER_LOG_TRACE_L1(_logger, args...) QUILL_LOG_TRACE_L1(_logger->logger(), args)
#define LOGGER_LOG_TRACE(_logger, args...) QUILL_LOG_TRACE_L1(_logger->logger(), args)
#define LOGGER_LOG_DEBUG(_logger, args...) QUILL_LOG_DEBUG(_logger->logger(), args)
#define LOGGER_LOG_WARN(_logger, args...) QUILL_LOG_WARNING(_logger->logger(), args)
#define LOGGER_LOG_INFO(_logger, args...) QUILL_LOG_INFO(_logger->logger(), args)
#define LOGGER_LOG_ERROR(_logger, args...) QUILL_LOG_ERROR(_logger->logger(), args)
#define LOGGER_LOG_FATAL(_logger, args...) QUILL_LOG_CRITICAL(_logger->logger(), args)

#define LOG_TRACE_L3(args...) LOGGER_LOG_TRACE_L3(XY_LOGGER, args)
#define LOG_TRACE_L2(args...) LOGGER_LOG_TRACE_L2(XY_LOGGER, args)
#define LOG_TRACE_L1(args...) LOGGER_LOG_TRACE_L1(XY_LOGGER, args)
#define LOG_TRACE(args...) LOGGER_LOG_TRACE(XY_LOGGER, args)
#define LOG_DEBUG(args...) LOGGER_LOG_DEBUG(XY_LOGGER, args)
#define LOG_WARN(args...) LOGGER_LOG_WARN(XY_LOGGER, args)
#define LOG_INFO(args...) LOGGER_LOG_INFO(XY_LOGGER, args)
#define LOG_ERROR(args...) LOGGER_LOG_ERROR(XY_LOGGER, args)
#define LOG_FATAL(args...) LOGGER_LOG_FATAL(XY_LOGGER, args)

#define LOG_COUNTER _XY_log_counter
#define LOGGER_LOG_EVERY_N(_logger, _loglevel, N, args...) \
  do {                                                     \
    thread_local uint64_t LOG_COUNTER = 0;                 \
    if (LOG_COUNTER % N == 0) {                            \
      LOGGER_LOG_##_loglevel(_logger, args);               \
    }                                                      \
    LOG_COUNTER++;                                         \
  } while (0)

#define LOG_EVERY_N(_loglevel, N, args...) LOGGER_LOG_EVERY_N(XY_LOGGER, _loglevel, N, args)

#define LOGGER_LOG_TRACE_EVERY_N(_logger, N, args...) LOGGER_LOG_EVERY_N(_logger, TRACE, N, args)
#define LOGGER_LOG_DEBUG_EVERY_N(_logger, N, args...) LOGGER_LOG_EVERY_N(_logger, DEBUG, N, args)
#define LOGGER_LOG_WARN_EVERY_N(_logger, N, args...) LOGGER_LOG_EVERY_N(_logger, WARN, N, args)
#define LOGGER_LOG_INFO_EVERY_N(_logger, N, args...) LOGGER_LOG_EVERY_N(_logger, INFO, N, args)
#define LOGGER_LOG_ERROR_EVERY_N(_logger, N, args...) LOGGER_LOG_EVERY_N(_logger, ERROR, N, args)
#define LOGGER_LOG_FATAL_EVERY_N(_logger, N, args...) LOGGER_LOG_EVERY_N(_logger, FATAL, N, args)

#define LOG_TRACE_EVERY_N(N, args...) LOG_EVERY_N(TRACE, N, args)
#define LOG_DEBUG_EVERY_N(N, args...) LOG_EVERY_N(DEBUG, N, args)
#define LOG_WARN_EVERY_N(N, args...) LOG_EVERY_N(WARN, N, args)
#define LOG_INFO_EVERY_N(N, args...) LOG_EVERY_N(INFO, N, args)
#define LOG_ERROR_EVERY_N(N, args...) LOG_EVERY_N(ERROR, N, args)
#define LOG_FATAL_EVERY_N(N, args...) LOG_EVERY_N(FATAL, N, args)

#define LOGGER_LOG_INTERVAL(_logger, _loglevel, interval_us, args...)         \
  do {                                                                        \
    thread_local std::chrono::time_point<std::chrono::steady_clock> limit_us; \
    const auto now = std::chrono::steady_clock::now();                        \
    if (now < limit_us) {                                                     \
      break;                                                                  \
    }                                                                         \
    limit_us = now + std::chrono::microseconds{interval_us};                  \
    LOGGER_LOG_##_loglevel(_logger, args);                                    \
  } while (0)

#define LOG_INTERVAL(_loglevel, N, args...) \
  LOGGER_LOG_INTERVAL(XY_LOGGER, _loglevel, interval_us, args)

#define LOGGER_LOG_TRACE_INTERVAL(_logger, interval_us, args...) \
  LOGGER_LOG_INTERVAL(_logger, TRACE, interval_us, args)
#define LOGGER_LOG_DEBUG_INTERVAL(_logger, interval_us, args...) \
  LOGGER_LOG_INTERVAL(_logger, DEBUG, interval_us, args)
#define LOGGER_LOG_WARN_INTERVAL(_logger, interval_us, args...) \
  LOGGER_LOG_INTERVAL(_logger, WARN, interval_us, args)
#define LOGGER_LOG_INFO_INTERVAL(_logger, interval_us, args...) \
  LOGGER_LOG_INTERVAL(_logger, INFO, interval_us, args)
#define LOGGER_LOG_ERROR_INTERVAL(_logger, interval_us, args...) \
  LOGGER_LOG_INTERVAL(_logger, ERROR, interval_us, args)
#define LOGGER_LOG_FATAL_INTERVAL(_logger, interval_us, args...) \
  LOGGER_LOG_INTERVAL(_logger, FATAL, interval_us, args)

#define LOG_TRACE_INTERVAL(interval_us, args...) LOG_INTERVAL(TRACE, interval_us, args)
#define LOG_DEBUG_INTERVAL(interval_us, args...) LOG_INTERVAL(DEBUG, interval_us, args)
#define LOG_WARN_INTERVAL(interval_us, args...) LOG_INTERVAL(WARN, interval_us, args)
#define LOG_INFO_INTERVAL(interval_us, args...) LOG_INTERVAL(INFO, interval_us, args)
#define LOG_ERROR_INTERVAL(interval_us, args...) LOG_INTERVAL(ERROR, interval_us, args)
#define LOG_FATAL_INTERVAL(interval_us, args...) LOG_INTERVAL(FATAL, interval_us, args)

// 对于自定义的可安全拷贝到后端进行format的类型，使用MARK_AS_COPY_LOGGABLE标记
// 在namespace外面使用
#define MARK_AS_COPY_LOGGABLE(type) \
  template <>                       \
  struct quill::copy_loggable<type> : std::true_type {}

}  // namespace xyts
