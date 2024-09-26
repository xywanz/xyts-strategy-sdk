#pragma once

#include <chrono>
#include <memory>
#include <type_traits>

#include "quill/Quill.h"
#include "xyts/core/formatter.h"
#include "xyu/utility.h"

namespace xyts {

class XyLogger : public xyu::NonCopyableNonMoveable {
 public:
  explicit XyLogger(quill::Logger* logger) : logger_(logger) {}

  quill::Logger* logger() { return logger_; }
  std::string loglevel() const;

 private:
  quill::Logger* logger_ = nullptr;
};

class XyGlobalLogger : public xyu::NonCopyableNonMoveable {
 public:
  ~XyGlobalLogger();

  static XyGlobalLogger& Instance() {
    static XyGlobalLogger logger;
    return logger;
  }

  // logfile: 日志文件名或console, {logfile}_2021-07-19.log
  // loglevel: 日志级别, trace|debug|info|warning|error|fatal
  void Init(const std::string& logfile, const std::string& loglevel = "info");

  // dynamically set log level. thread safe
  bool set_log_level(const std::string& loglevel);

  std::string log_level() const;

  XyLogger* logger() { return logger_.get(); }

 private:
  XyGlobalLogger();

  std::unique_ptr<XyLogger> logger_ = nullptr;
};

void InitGlobalLogger(const std::string& logfile, const std::string& loglevel = "info");

void FlushLog();

std::shared_ptr<XyLogger> GetCustomLogger(const std::string& logfile);

#define XY_GLOBAL_LOGGER ::xyts::XyGlobalLogger::Instance().logger()

#define LOGGER_LOG_TRACE_L3(_logger, args...) QUILL_LOG_TRACE_L3(_logger->logger(), args)
#define LOGGER_LOG_TRACE_L2(_logger, args...) QUILL_LOG_TRACE_L2(_logger->logger(), args)
#define LOGGER_LOG_TRACE_L1(_logger, args...) QUILL_LOG_TRACE_L1(_logger->logger(), args)
#define LOGGER_LOG_TRACE(_logger, args...) QUILL_LOG_TRACE_L1(_logger->logger(), args)
#define LOGGER_LOG_DEBUG(_logger, args...) QUILL_LOG_DEBUG(_logger->logger(), args)
#define LOGGER_LOG_WARNING(_logger, args...) QUILL_LOG_WARNING(_logger->logger(), args)
#define LOGGER_LOG_INFO(_logger, args...) QUILL_LOG_INFO(_logger->logger(), args)
#define LOGGER_LOG_ERROR(_logger, args...) QUILL_LOG_ERROR(_logger->logger(), args)
#define LOGGER_LOG_FATAL(_logger, args...) QUILL_LOG_CRITICAL(_logger->logger(), args)

#define LOG_TRACE_L3(args...) LOGGER_LOG_TRACE_L3(XY_GLOBAL_LOGGER, args)
#define LOG_TRACE_L2(args...) LOGGER_LOG_TRACE_L2(XY_GLOBAL_LOGGER, args)
#define LOG_TRACE_L1(args...) LOGGER_LOG_TRACE_L1(XY_GLOBAL_LOGGER, args)
#define LOG_TRACE(args...) LOGGER_LOG_TRACE(XY_GLOBAL_LOGGER, args)
#define LOG_DEBUG(args...) LOGGER_LOG_DEBUG(XY_GLOBAL_LOGGER, args)
#define LOG_WARNING(args...) LOGGER_LOG_WARNING(XY_GLOBAL_LOGGER, args)
#define LOG_INFO(args...) LOGGER_LOG_INFO(XY_GLOBAL_LOGGER, args)
#define LOG_ERROR(args...) LOGGER_LOG_ERROR(XY_GLOBAL_LOGGER, args)
#define LOG_FATAL(args...) LOGGER_LOG_FATAL(XY_GLOBAL_LOGGER, args)

#define LOGGER_LOG_EVERY_N(logger, loglevel, N, args...) \
  do {                                                   \
    thread_local std::size_t xy_log_counter = 0;         \
    if (xy_log_counter % N == 0) {                       \
      LOGGER_LOG_##loglevel(logger, args);               \
    }                                                    \
    xy_log_counter++;                                    \
  } while (0)

#define LOG_EVERY_N(loglevel, N, args...) LOGGER_LOG_EVERY_N(XY_GLOBAL_LOGGER, loglevel, N, args)

#define LOGGER_LOG_TRACE_EVERY_N(logger, N, args...) LOGGER_LOG_EVERY_N(logger, TRACE, N, args)
#define LOGGER_LOG_DEBUG_EVERY_N(logger, N, args...) LOGGER_LOG_EVERY_N(logger, DEBUG, N, args)
#define LOGGER_LOG_WARNING_EVERY_N(logger, N, args...) LOGGER_LOG_EVERY_N(logger, WARNING, N, args)
#define LOGGER_LOG_INFO_EVERY_N(logger, N, args...) LOGGER_LOG_EVERY_N(logger, INFO, N, args)
#define LOGGER_LOG_ERROR_EVERY_N(logger, N, args...) LOGGER_LOG_EVERY_N(logger, ERROR, N, args)
#define LOGGER_LOG_FATAL_EVERY_N(logger, N, args...) LOGGER_LOG_EVERY_N(logger, FATAL, N, args)

#define LOG_TRACE_EVERY_N(N, args...) LOG_EVERY_N(TRACE, N, args)
#define LOG_DEBUG_EVERY_N(N, args...) LOG_EVERY_N(DEBUG, N, args)
#define LOG_WARNING_EVERY_N(N, args...) LOG_EVERY_N(WARNING, N, args)
#define LOG_INFO_EVERY_N(N, args...) LOG_EVERY_N(INFO, N, args)
#define LOG_ERROR_EVERY_N(N, args...) LOG_EVERY_N(ERROR, N, args)
#define LOG_FATAL_EVERY_N(N, args...) LOG_EVERY_N(FATAL, N, args)

#define LOGGER_LOG_INTERVAL(logger, loglevel, interval, args...)           \
  do {                                                                     \
    thread_local std::chrono::time_point<std::chrono::steady_clock> limit; \
    const auto now = std::chrono::steady_clock::now();                     \
    if (now < limit) {                                                     \
      break;                                                               \
    }                                                                      \
    limit = now + interval;                                                \
    LOGGER_LOG_##loglevel(logger, args);                                   \
  } while (0)

#define LOG_INTERVAL(loglevel, interval, args...) \
  LOGGER_LOG_INTERVAL(XY_GLOBAL_LOGGER, loglevel, interval, args)

#define LOGGER_LOG_TRACE_INTERVAL(logger, interval, args...) \
  LOGGER_LOG_INTERVAL(logger, TRACE, interval, args)
#define LOGGER_LOG_DEBUG_INTERVAL(logger, interval, args...) \
  LOGGER_LOG_INTERVAL(logger, DEBUG, interval, args)
#define LOGGER_LOG_WARNING_INTERVAL(logger, interval, args...) \
  LOGGER_LOG_INTERVAL(logger, WARNING, interval, args)
#define LOGGER_LOG_INFO_INTERVAL(logger, interval, args...) \
  LOGGER_LOG_INTERVAL(logger, INFO, interval, args)
#define LOGGER_LOG_ERROR_INTERVAL(logger, interval, args...) \
  LOGGER_LOG_INTERVAL(logger, ERROR, interval, args)
#define LOGGER_LOG_FATAL_INTERVAL(logger, interval, args...) \
  LOGGER_LOG_INTERVAL(logger, FATAL, interval, args)

#define LOG_TRACE_INTERVAL(interval, args...) LOG_INTERVAL(TRACE, interval, args)
#define LOG_DEBUG_INTERVAL(interval, args...) LOG_INTERVAL(DEBUG, interval, args)
#define LOG_WARNING_INTERVAL(interval, args...) LOG_INTERVAL(WARNING, interval, args)
#define LOG_INFO_INTERVAL(interval, args...) LOG_INTERVAL(INFO, interval, args)
#define LOG_ERROR_INTERVAL(interval, args...) LOG_INTERVAL(ERROR, interval, args)
#define LOG_FATAL_INTERVAL(interval, args...) LOG_INTERVAL(FATAL, interval, args)

// 对于自定义的可安全拷贝到后端进行format的类型，使用MARK_AS_COPY_LOGGABLE标记
#define MARK_AS_COPY_LOGGABLE(type) \
  template <>                       \
  struct quill::copy_loggable<type> : std::true_type {}

}  // namespace xyts
