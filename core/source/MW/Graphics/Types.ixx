/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Types;
import Microwave.Graphics.Color;
import Microwave.Math;
import Microwave.System.Json;
import <unordered_map>;
import <string>;

export namespace mw {
inline namespace gfx {

struct UIVertex
{
    Vec3 pos;
    Vec2 uv;
    Color col;
};

struct Box
{
    float left;
    float top;
    float right;
    float bottom;

    Box()
        : left(0), top(0), right(0), bottom(0) {}

    Box(float left, float top, float right, float bottom)
        : left(left), top(top), right(right), bottom(bottom) {}
};

constexpr int AlignBitsLeft = 0b00000001;
constexpr int AlignBitsCenter = 0b00000010;
constexpr int AlignBitsRight = 0b00000100;
constexpr int AlignBitsJustify = 0b00001000;
constexpr int AlignBitsTop = 0b00010000;
constexpr int AlignBitsMiddle = 0b00100000;
constexpr int AlignBitsBottom = 0b01000000;

enum class TextAlign : int
{
    TopLeft = AlignBitsTop | AlignBitsLeft,
    TopCenter = AlignBitsTop | AlignBitsCenter,
    TopRight = AlignBitsTop | AlignBitsRight,
    TopJustify = AlignBitsTop | AlignBitsJustify,
    MiddleLeft = AlignBitsMiddle | AlignBitsLeft,
    MiddleCenter = AlignBitsMiddle | AlignBitsCenter,
    MiddleRight = AlignBitsMiddle | AlignBitsRight,
    MiddleJustify = AlignBitsMiddle | AlignBitsJustify,
    BottomLeft = AlignBitsBottom | AlignBitsLeft,
    BottomCenter = AlignBitsBottom | AlignBitsCenter,
    BottomRight = AlignBitsBottom | AlignBitsRight,
    BottomJustify = AlignBitsBottom | AlignBitsJustify,
};

void to_json(json& obj, const UIVertex& v) {
    obj["pos"] = v.pos;
    obj["uv"] = v.uv;
    obj["col"] = v.col;
}

void from_json(const json& obj, UIVertex& v) {
    v.pos = obj.value("pos", v.pos);
    v.uv = obj.value("uv", v.uv);
    v.col = obj.value("col", v.col);
}

void to_json(json& obj, const Box& b) {
    obj["left"] = b.left;
    obj["top"] = b.top;
    obj["right"] = b.right;
    obj["bottom"] = b.bottom;
}

void from_json(const json& obj, Box& b) {
    b.left = obj.value("left", b.left);
    b.top = obj.value("top", b.top);
    b.right = obj.value("right", b.right);
    b.bottom = obj.value("bottom", b.bottom);
}

void to_json(json& obj, const TextAlign& ta)
{
    static std::unordered_map<TextAlign, std::string> names {
        { TextAlign::TopLeft, "TopLeft" },
        { TextAlign::TopCenter, "TopCenter" },
        { TextAlign::TopRight, "TopRight" },
        { TextAlign::TopJustify, "TopJustify" },
        { TextAlign::MiddleLeft, "MiddleLeft" },
        { TextAlign::MiddleCenter, "MiddleCenter" },
        { TextAlign::MiddleRight, "MiddleRight" },
        { TextAlign::MiddleJustify, "MiddleJustify" },
        { TextAlign::BottomLeft, "BottomLeft" },
        { TextAlign::BottomCenter, "BottomCenter" },
        { TextAlign::BottomRight, "BottomRight" },
        { TextAlign::BottomJustify, "BottomJustify" }
    };
    obj = names[ta];
}

void from_json(const json& obj, TextAlign& ta)
{
    static std::unordered_map<std::string, TextAlign> values {
        { "TopLeft", TextAlign::TopLeft },
        { "TopCenter", TextAlign::TopCenter },
        { "TopRight", TextAlign::TopRight },
        { "TopJustify", TextAlign::TopJustify },
        { "MiddleLeft", TextAlign::MiddleLeft },
        { "MiddleCenter", TextAlign::MiddleCenter },
        { "MiddleRight", TextAlign::MiddleRight },
        { "MiddleJustify", TextAlign::MiddleJustify },
        { "BottomLeft", TextAlign::BottomLeft },
        { "BottomCenter", TextAlign::BottomCenter },
        { "BottomRight", TextAlign::BottomRight },
        { "BottomJustify", TextAlign::BottomJustify }
    };
    ta = values[obj.get<std::string>()];
}

} // gfx
} // mw
