/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Json.h>
#include <MW/Graphics/GraphicsTypes.h>
#include <MW/Graphics/GraphicsResource.h>
#include <MW/Math/Math.h>
#include <MW/System/Pointers.h>
#include <cstdint>
#include <span>
#include <string>

namespace mw {
inline namespace gfx {

enum class WrapMode : int
{
    Clamp,
    Repeat,
};

enum class FilterMode : int
{
    Point,
    Bilinear,
    Trilinear,
};

class Texture : public GraphicsResource
{
public:
    virtual uintptr_t GetHandle() = 0;
    virtual PixelDataFormat GetFormat() const = 0;
    virtual IVec2 GetSize() const = 0;

    virtual bool IsDynamic() const = 0;

    virtual void SetPixels(const std::span<std::byte>& data) = 0;
    virtual void SetPixels(const std::span<std::byte>& data, const IntRect& rect) = 0;

    virtual void SetWrapMode(WrapMode mode) = 0;
    virtual WrapMode GetWrapMode() const = 0;

    virtual void SetFilterMode(FilterMode mode) = 0;
    virtual FilterMode GetFilterMode() const = 0;

    virtual void SetAnisoLevel(float level) = 0;
    virtual float GetAnisoLevel() const = 0;
};

inline void to_json(json& obj, const WrapMode& wrapMode) {
    static const char* modeNames[]{ "Clamp", "Repeat", };
    obj = modeNames[(int)wrapMode];
}

inline void from_json(const json& obj, WrapMode& wrapMode) {
    static std::unordered_map<std::string, WrapMode> modes{
        { "Clamp", WrapMode::Clamp },
        { "Repeat", WrapMode::Repeat }
    };
    wrapMode = modes[obj.get<std::string>("Repeat")];
}

inline void to_json(json& obj, const FilterMode& filterMode) {
    static const char* modeNames[]{ "Point", "Bilinear", "Trilinear" };
    obj = modeNames[(int)filterMode];
}

inline void from_json(const json& obj, FilterMode& filterMode) {
    static std::unordered_map<std::string, FilterMode> modes{
        { "Point", FilterMode::Point },
        { "Bilinear", FilterMode::Bilinear },
        { "Trilinear", FilterMode::Trilinear }
    };
    filterMode = modes[obj.get<std::string>("Bilinear")];
}

}
}
