// Minimal spdlog shim for the RXDK/Xbox libultraship build.
//
// The plan drops spdlog+fmt (heavy, spawns a logging thread) for the console port.
// libultraship's logging is diagnostic-only, so the SPDLOG_* macros become no-ops and
// the spdlog API surface Context.cpp constructs (loggers, sinks, async pool) is stubbed
// just enough to compile and behave inertly. Real formatting (fmt::format) is only used
// by files we don't build on Xbox (Metal backend, ImGui debugger). Enhance later if a
// serial log channel proves worth wiring.
#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <exception>
#include <cstddef>
#include "fmt/fmt.h"

namespace spdlog {
// spdlog aliases the fmt library as spdlog::fmt_lib; some SoH code (Presets) formats via it.
namespace fmt_lib = ::fmt;

namespace level {
enum level_enum { trace = 0, debug, info, warn, err, critical, off, n_levels };
inline std::string_view to_string_view(level_enum l) {
    switch (l) {
        case trace: return "trace";
        case debug: return "debug";
        case info: return "info";
        case warn: return "warning";
        case err: return "error";
        case critical: return "critical";
        default: return "off";
    }
}
} // namespace level

enum class async_overflow_policy { block, overrun_oldest, discard_new };

struct source_loc {
    const char* filename = nullptr;
    int line = 0;
    const char* funcname = nullptr;
    source_loc() = default;
    source_loc(const char* f, int l, const char* fn) : filename(f), line(l), funcname(fn) {}
    bool empty() const noexcept { return line == 0; }
};

class spdlog_ex : public std::exception {
    std::string mMsg;
  public:
    explicit spdlog_ex(std::string msg) : mMsg(std::move(msg)) {}
    const char* what() const noexcept override { return mMsg.c_str(); }
};

namespace sinks {
class sink {
  public:
    virtual ~sink() = default;
    void set_level(level::level_enum) {}
    void set_pattern(const std::string&) {}
};
} // namespace sinks

using sink_ptr = std::shared_ptr<sinks::sink>;

namespace details {
class thread_pool {
  public:
    thread_pool() = default;
    thread_pool(std::size_t /*qSize*/, std::size_t /*threads*/) {}
    thread_pool(std::size_t, std::size_t, std::size_t) {}
};
} // namespace details

class logger {
  public:
    logger() = default;
    explicit logger(std::string) {}
    template <class It> logger(std::string, It, It) {}
    template <class It> logger(std::string, It, It, level::level_enum) {}
    virtual ~logger() = default;

    void set_level(level::level_enum) {}
    level::level_enum level() const { return level::off; }
    void flush() {}
    void flush_on(level::level_enum) {}
    void set_pattern(const std::string&) {}
    const std::string& name() const {
        static const std::string n;
        return n;
    }

    template <class... A> void trace(A&&...) {}
    template <class... A> void debug(A&&...) {}
    template <class... A> void info(A&&...) {}
    template <class... A> void warn(A&&...) {}
    template <class... A> void error(A&&...) {}
    template <class... A> void critical(A&&...) {}
    template <class... A> void log(level::level_enum, A&&...) {}
    template <class... A> void log(source_loc, level::level_enum, A&&...) {}
    bool should_log(level::level_enum) const { return false; }
};

class async_logger : public logger {
  public:
    template <class It>
    async_logger(std::string name, It b, It e, std::shared_ptr<details::thread_pool>,
                 async_overflow_policy = async_overflow_policy::block)
        : logger(std::move(name), b, e) {}
};

inline std::shared_ptr<logger>& _rxdk_default_logger() {
    static std::shared_ptr<logger> inst = std::make_shared<logger>();
    return inst;
}
inline std::shared_ptr<logger> default_logger() { return _rxdk_default_logger(); }
inline logger* default_logger_raw() { return _rxdk_default_logger().get(); }
inline std::shared_ptr<logger> get(const std::string&) { return _rxdk_default_logger(); }
inline void register_logger(std::shared_ptr<logger>) {}
inline void set_default_logger(std::shared_ptr<logger> l) { _rxdk_default_logger() = l; }
inline void set_level(level::level_enum) {}
inline void set_pattern(const std::string&) {}
inline void flush_every(...) {}
inline void shutdown() {}
inline void init_thread_pool(std::size_t, std::size_t) {}

template <class... A> void trace(A&&...) {}
template <class... A> void debug(A&&...) {}
template <class... A> void info(A&&...) {}
template <class... A> void warn(A&&...) {}
template <class... A> void error(A&&...) {}
template <class... A> void critical(A&&...) {}

} // namespace spdlog

// --- Log macros: no-ops. Args are format-only (no side effects), so discarding is safe. ---
#define SPDLOG_LEVEL_TRACE 0
#define SPDLOG_LEVEL_DEBUG 1
#define SPDLOG_LEVEL_INFO 2
#define SPDLOG_LEVEL_WARN 3
#define SPDLOG_LEVEL_ERROR 4
#define SPDLOG_LEVEL_CRITICAL 5
#define SPDLOG_LEVEL_OFF 6

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#endif

#ifndef SPDLOG_FUNCTION
#define SPDLOG_FUNCTION __FUNCTION__
#endif

#define SPDLOG_TRACE(...) (void)0
#define SPDLOG_DEBUG(...) (void)0
#define SPDLOG_INFO(...) (void)0
#define SPDLOG_WARN(...) (void)0
#define SPDLOG_ERROR(...) (void)0
#define SPDLOG_CRITICAL(...) (void)0

#define SPDLOG_LOGGER_TRACE(...) (void)0
#define SPDLOG_LOGGER_DEBUG(...) (void)0
#define SPDLOG_LOGGER_INFO(...) (void)0
#define SPDLOG_LOGGER_WARN(...) (void)0
#define SPDLOG_LOGGER_ERROR(...) (void)0
#define SPDLOG_LOGGER_CRITICAL(...) (void)0
