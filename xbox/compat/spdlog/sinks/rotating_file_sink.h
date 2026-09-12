// spdlog rotating file sink shim -> inert (no file I/O for logs on Xbox).
#pragma once
#include "spdlog/spdlog.h"
#include <string>
namespace spdlog { namespace sinks {
class rotating_file_sink_mt : public sink {
  public:
    rotating_file_sink_mt(const std::string&, std::size_t, std::size_t, bool = false) {}
};
class rotating_file_sink_st : public sink {
  public:
    rotating_file_sink_st(const std::string&, std::size_t, std::size_t, bool = false) {}
};
}} // namespace
