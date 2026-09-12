// spdlog stdout color sink shim -> inert.
#pragma once
#include "spdlog/spdlog.h"
namespace spdlog { namespace sinks {
class stdout_color_sink_mt : public sink { public: stdout_color_sink_mt() = default; };
class stdout_color_sink_st : public sink { public: stdout_color_sink_st() = default; };
class stderr_color_sink_mt : public sink { public: stderr_color_sink_mt() = default; };
}} // namespace
