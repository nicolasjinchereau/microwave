/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Json.h>
#include <MW/Utilities/EnumFlags.h>

namespace mw {
inline namespace scene {

enum class LayerMask : uint32_t
{
    None    = 0,
    All     = 0xFFFFFFFF,
    Default = 1u << 0,
    UI      = 1u << 1,
    User2   = 1u << 2,
    User3   = 1u << 3,
    User4   = 1u << 4,
    User5   = 1u << 5,
    User6   = 1u << 6,
    User7   = 1u << 7,
    User8   = 1u << 8,
    User9   = 1u << 9,
    User10  = 1u << 10,
    User11  = 1u << 11,
    User12  = 1u << 12,
    User13  = 1u << 13,
    User14  = 1u << 14,
    User15  = 1u << 15,
    User16  = 1u << 16,
    User17  = 1u << 17,
    User18  = 1u << 18,
    User19  = 1u << 19,
    User20  = 1u << 20,
    User21  = 1u << 21,
    User22  = 1u << 22,
    User23  = 1u << 23,
    User24  = 1u << 24,
    User25  = 1u << 25,
    User26  = 1u << 26,
    User27  = 1u << 27,
    User28  = 1u << 28,
    User29  = 1u << 29,
    User30  = 1u << 30,
    User31  = 1u << 31
};
ENUM_FLAGS(LayerMask);

inline void to_json(json& obj, const LayerMask& mask) {
    obj = (uint32_t)mask;
}

inline void from_json(const json& obj, LayerMask& mask) {
    mask = (LayerMask)obj.get<uint32_t>();
}

} // scene
} // mw
