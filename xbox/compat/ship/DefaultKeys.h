#pragma once

#include <string_view>
#include <array>
#include <cstddef>

// Auto-generated file containing public keys

struct DefaultKey {
    std::string_view name;
    std::string_view data;
};

inline constexpr std::string_view HM64_data = "42bdede3cf6ebc2bd08a195c6ae6e7cca6c492a88759f2aa0521ee41999170c1";
inline constexpr std::string_view Kenix1_data = "929522494d18cd66a34a2cfc7adc2254d7ec88bceeb566c6f7c90595d0746017";
inline constexpr std::string_view Kenix2_data = "04993c2d77b92704d8682d7112476bcdafc64bbdb4a067a512ca2c4908e69546";
inline constexpr std::string_view Kenix3_data = "0e8409b24c415680bb7b0e91921286df234a30a408bfcae510b726be4177c2bd";
inline constexpr std::string_view Kenix4_data = "3649c7922512da830e9130a8208fc1170dca586ba0c24a92eaa757a082432c02";
inline constexpr std::string_view Kenix5_data = "9a1ba2edc0521a8e6cb6e69666b42f92a4d25843122a75802382da36de50ca31";

// Table of all public keys
inline constexpr std::array<DefaultKey, 6> AllDefaultKeys = {
    DefaultKey{ "HM64", HM64_data },
    DefaultKey{ "Kenix1", Kenix1_data },
    DefaultKey{ "Kenix2", Kenix2_data },
    DefaultKey{ "Kenix3", Kenix3_data },
    DefaultKey{ "Kenix4", Kenix4_data },
    DefaultKey{ "Kenix5", Kenix5_data },
};

inline constexpr std::size_t AllDefaultKeysSize = 6;
