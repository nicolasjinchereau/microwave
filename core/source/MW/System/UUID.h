/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Json.h>
#include <cstdint>
#include <array>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>
#include <cassert>

namespace mw {
inline namespace system {

class UUID
{
    inline static uint64_t GetSeed() {
        return (uint64_t)std::chrono::system_clock::now().time_since_epoch().count();
    }

    inline static std::mt19937_64 generator{ GetSeed() };

    std::array<uint8_t, 16> data = {};

public:
    static void SetRandomSeed(uint64_t seed) {
        generator.seed(seed);
    }

    static UUID New()
    {
        UUID ret;

        int i = 0;
        uint64_t val = generator();
        for (auto it = ret.data.begin(); it != ret.data.end(); ++it, ++i)
        {
            if (i == sizeof(uint64_t)) {
                val = generator();
                i = 0;
            }

            *it = (uint8_t)((val >> (i * 8)) & 0xFF);
        }

        // set variant to 0b10xxxxxx
        ret.data[8] &= 0xBF;
        ret.data[8] |= 0x80;

        // set version to 4
        ret.data[6] &= 0x4F; // 0b01001111
        ret.data[6] |= 0x40; // 0b01000000

        return ret;
    }

    std::string ToString() const
    {
        char buff[33];

        for (int i = 0; i != data.size(); ++i)
        {
            uint8_t byte = data[i];
            static const char* HexChars = "0123456789abcdef";
            buff[i * 2 + 0] = HexChars[(byte >> 4) & 0x0F];
            buff[i * 2 + 1] = HexChars[byte & 0x0F];
        }

        buff[32] = 0;

        return buff;
    }

    static UUID FromString(const std::string& str)
    {
        UUID ret;

        if (str.size() == 32)
        {
            static const char* HexChars = "0123456789abcdefABCDEF";

            for (size_t i = 0; i != 32; i += 2)
            {
                char hex[3] = { str[i], str[i + 1], 0 };

                if (!strchr(HexChars, hex[0]) || !strchr(HexChars, hex[1]))
                {
                    ret.data.fill(0);
                    break;
                }

                int x = std::stoi(hex, nullptr, 16);
                ret.data[i / 2] = (uint8_t)x;
            }
        }

        return ret;
    }

    std::size_t GetHash() const noexcept {
        auto view = std::string_view((const char*)data.data(), data.size());
        return std::hash<std::string_view>()(view);
    }

    bool operator==(const UUID& u) const {
        return memcmp(data.data(), u.data.data(), u.data.size()) == 0;
    }

    bool operator!=(const UUID& u) const {
        return !(*this == u);
    }

    bool empty() const {
        return std::all_of(data.begin(), data.end(), [](uint8_t b) { return b == 0; });
    }

    operator bool() const {
        return !empty();
    }
};

inline void to_json(json& obj, const UUID& uuid) {
    obj = uuid.ToString();
}

inline void from_json(const json& obj, UUID& uuid) {
    uuid = UUID::FromString(obj.get<std::string>("00000000000000000000000000000000"));
}

} // system
} // mw

namespace std
{
template<>
struct hash<mw::UUID>
{
    std::size_t operator()(const mw::UUID& u) const {
        return u.GetHash();
    }
};
}
