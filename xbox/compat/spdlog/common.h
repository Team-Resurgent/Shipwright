// spdlog/common.h shim -> the shim's core types live in spdlog.h (level_enum, source_loc,
// logger, etc.). Re-expose them for code that includes <spdlog/common.h> directly.
#pragma once
#include "spdlog/spdlog.h"
namespace spdlog { using string_view_t = std::string; using memory_buf_t = std::string; }
