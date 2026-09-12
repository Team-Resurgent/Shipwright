// spdlog async shim -> see spdlog.h. Async logging collapses to the inert base logger.
#pragma once
#include "spdlog/spdlog.h"
namespace spdlog {
template <class... A> inline std::shared_ptr<logger> create_async(A&&...) { return default_logger(); }
template <class... A> inline std::shared_ptr<logger> create_async_nb(A&&...) { return default_logger(); }
} // namespace spdlog
