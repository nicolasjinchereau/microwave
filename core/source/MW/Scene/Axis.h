/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Json.h>
#include <string>
#include <unordered_map>

namespace mw {
inline namespace scene {

enum class Axis
{
    X,
    Y,
    Z
};

inline void to_json(json& obj, const Axis& upAxis)
{
    static const char* axisNames[]{ "X", "Y", "Z" };
    obj = axisNames[(int)upAxis];
}

inline void from_json(const json& obj, Axis& upAxis)
{
    static std::unordered_map<std::string, Axis> axes{
        { "X", Axis::X },
        { "Y", Axis::Y },
        { "Z", Axis::Z }
    };
    upAxis = axes[obj.get<std::string>("Invalid")];
}

} // scene
} // mw
