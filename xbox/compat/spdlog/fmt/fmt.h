// Minimal fmt shim -> only referenced by files not built on Xbox (Metal/ImGui debugger).
// Provides a degenerate format() that returns the format string so those TUs would still
// compile if ever included; substitution is intentionally not implemented.
#pragma once
#include <string>
#include <utility>
namespace fmt {
template <class... A> inline std::string format(const char* f, A&&...) { return std::string(f); }
template <class... A> inline std::string format(const std::string& f, A&&...) { return f; }
} // namespace fmt
