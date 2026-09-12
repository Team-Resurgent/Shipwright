// Compile-check TU: proves nlohmann/json.hpp instantiates under the RXDK C++ toolchain
// (exceptions on, freestanding libc++). Not part of the runtime; a canary only.
#include <nlohmann/json.hpp>

extern "C" int rxdk_json_selftest(void) {
    nlohmann::json j;
    j["ship"] = "harkinian";
    j["ram"] = 128;
    auto arr = nlohmann::json::array();
    arr.push_back(1);
    arr.push_back(2);
    j["list"] = arr;
    std::string s = j.dump();
    return (int)s.size();
}
