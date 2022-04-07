/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <span>
#include <string>
#include <vector>

namespace mw {
inline namespace utilities {

class Base64
{
public:
    static constexpr std::string_view Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    static std::string Encode(std::span<const std::byte> in)
    {
        std::string out;

        int val = 0;
        int valb = -6;

        for (std::byte c : in)
        {
            val = (val << 8) + (int)c;
            valb += 8;

            while (valb >= 0)
            {
                out.push_back(Alphabet[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }

        if (valb > -6)
            out.push_back(Alphabet[((val << 8) >> (valb + 8)) & 0x3F]);

        while (out.size() % 4)
            out.push_back('=');

        return out;
    }

    static std::vector<std::byte> Decode(const std::string& in)
    {
        std::vector<std::byte> out;

        std::array<int, 256> lookup;
        std::fill(lookup.begin(), lookup.end(), -1);

        for (int i = 0; i < 64; i++)
            lookup[Alphabet[i]] = i;

        int val = 0;
        int valb = -8;

        out.reserve(in.size());

        for (uint8_t c : in)
        {
            if (lookup[c] == -1)
                break;

            val = (val << 6) + lookup[c];
            valb += 6;

            if (valb >= 0)
            {
                out.push_back((std::byte)((val >> valb) & 0xFF));
                valb -= 8;
            }
        }

        return out;
    }
};

} // utilities
} // mw
