/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Json.h>

namespace mw {
inline namespace gfx {

enum class GraphicsDriverType : int
{
    None,
    Default,
    Direct3D11,
    Direct3D12,
    OpenGL,
    OpenGLES2,
    OpenGLES3,
    Metal,
    Vulkan
};

enum class DrawMode : int
{
    Points,
    Lines,
    LineStrip,
    Triangles,
    TriangleStrip
};

enum class CullMode : int
{
    Off,
    Back,
    Front,
};

enum class DepthTest : int
{
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always
};

enum class BlendFactor : int
{
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    SrcAlphaSaturate,
    ConstColor,
    OneMinusConstColor,
    Src1Color,
    OneMinusSrc1Color,
    Src1Alpha,
    OneMinusSrc1Alpha
};

enum class BlendOperation : int
{
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
};

enum class PixelDataFormat
{
    Unspecified,
    Alpha8,
    RGB24,
    RGBA32,
    RGBAFloat
};

inline size_t GetBytesPerPixel(PixelDataFormat format)
{
    switch(format)
    {
    case PixelDataFormat::Alpha8:    return 1;
    case PixelDataFormat::RGB24:     return 3;
    case PixelDataFormat::RGBA32:    return 4;
    case PixelDataFormat::RGBAFloat: return 16;
    default:                         return 0;
    }
}

void to_json(json& obj, const GraphicsDriverType& type);
void from_json(const json& obj, GraphicsDriverType& type);
void to_json(json& obj, const DrawMode& type);
void from_json(const json& obj, DrawMode& type);
void to_json(json& obj, const CullMode& type);
void from_json(const json& obj, CullMode& type);
void to_json(json& obj, const DepthTest& type);
void from_json(const json& obj, DepthTest& type);
void to_json(json& obj, const BlendFactor& type);
void from_json(const json& obj, BlendFactor& type);
void to_json(json& obj, const BlendOperation& type);
void from_json(const json& obj, BlendOperation& type);
void to_json(json& obj, const PixelDataFormat& type);
void from_json(const json& obj, PixelDataFormat& type);

inline void to_json(json& obj, const GraphicsDriverType& type)
{
    static std::unordered_map<GraphicsDriverType, std::string> names {
        { GraphicsDriverType::None, "None" },
        { GraphicsDriverType::Default, "Default" },
        { GraphicsDriverType::Direct3D11, "Direct3D11" },
        { GraphicsDriverType::Direct3D12, "Direct3D12" },
        { GraphicsDriverType::OpenGL, "OpenGL" },
        { GraphicsDriverType::OpenGLES2, "OpenGLES2" },
        { GraphicsDriverType::OpenGLES3, "OpenGLES3" },
        { GraphicsDriverType::Metal, "Metal" },
        { GraphicsDriverType::Vulkan, "Vulkan" }
    };

    obj = names[type];
}

inline void from_json(const json& obj, GraphicsDriverType& type)
{
    static std::unordered_map<std::string, GraphicsDriverType> types {
        { "None", GraphicsDriverType::None },
        { "Default", GraphicsDriverType::Default },
        { "Direct3D11", GraphicsDriverType::Direct3D11 },
        { "Direct3D12", GraphicsDriverType::Direct3D12 },
        { "OpenGL", GraphicsDriverType::OpenGL },
        { "OpenGLES2", GraphicsDriverType::OpenGLES2 },
        { "OpenGLES3", GraphicsDriverType::OpenGLES3 },
        { "Metal", GraphicsDriverType::Metal },
        { "Vulkan", GraphicsDriverType::Vulkan }
    };

    auto val = obj.get<std::string>();
    type = types[val];
}

inline void to_json(json& obj, const DrawMode& type)
{
    static std::unordered_map<DrawMode, std::string> names {
        { DrawMode::Points, "Points" },
        { DrawMode::Lines, "Lines" },
        { DrawMode::LineStrip, "LineStrip" },
        { DrawMode::Triangles, "Triangles" },
        { DrawMode::TriangleStrip, "TriangleStrip" }
    };

    obj = names[type];
}

inline void from_json(const json& obj, DrawMode& type)
{
    static std::unordered_map<std::string, DrawMode> types {
        { "Points", DrawMode::Points },
        { "Lines", DrawMode::Lines },
        { "LineStrip", DrawMode::LineStrip },
        { "Triangles", DrawMode::Triangles },
        { "TriangleStrip", DrawMode::TriangleStrip }
    };

    auto val = obj.get<std::string>();
    type = types[val];
}

inline void to_json(json& obj, const CullMode& type)
{
    static std::unordered_map<CullMode, std::string> names{
        { CullMode::Off, "Off" },
        { CullMode::Back, "Back" },
        { CullMode::Front, "Front" }
    };

    obj = names[type];
}

inline void from_json(const json& obj, CullMode& type)
{
    static std::unordered_map<std::string, CullMode> types{
        { "Off", CullMode::Off },
        { "Back", CullMode::Back },
        { "Front", CullMode::Front }
    };

    auto val = obj.get<std::string>();
    type = types[val];
}

inline void to_json(json& obj, const DepthTest& type)
{
    static std::unordered_map<DepthTest, std::string> names {
        { DepthTest::Never, "Never" },
        { DepthTest::Less, "Less" },
        { DepthTest::Equal, "Equal" },
        { DepthTest::LessOrEqual, "LessOrEqual" },
        { DepthTest::Greater, "Greater" },
        { DepthTest::NotEqual, "NotEqual" },
        { DepthTest::GreaterOrEqual, "GreaterOrEqual" },
        { DepthTest::Always, "Always" }
    };

    obj = names[type];
}

inline void from_json(const json& obj, DepthTest& type)
{
    static std::unordered_map<std::string, DepthTest> types {
        { "Never", DepthTest::Never },
        { "Less", DepthTest::Less },
        { "Equal", DepthTest::Equal },
        { "LessOrEqual", DepthTest::LessOrEqual },
        { "Greater", DepthTest::Greater },
        { "NotEqual", DepthTest::NotEqual },
        { "GreaterOrEqual", DepthTest::GreaterOrEqual },
        { "Always", DepthTest::Always }
    };

    auto val = obj.get<std::string>();
    type = types[val];
}

inline void to_json(json& obj, const BlendFactor& type)
{
    static std::unordered_map<BlendFactor, std::string> names {
        { BlendFactor::Zero, "Zero" },
        { BlendFactor::One, "One" },
        { BlendFactor::SrcColor, "SrcColor" },
        { BlendFactor::OneMinusSrcColor, "OneMinusSrcColor" },
        { BlendFactor::DstColor, "DstColor" },
        { BlendFactor::OneMinusDstColor, "OneMinusDstColor" },
        { BlendFactor::SrcAlpha, "SrcAlpha" },
        { BlendFactor::OneMinusSrcAlpha, "OneMinusSrcAlpha" },
        { BlendFactor::DstAlpha, "DstAlpha" },
        { BlendFactor::OneMinusDstAlpha, "OneMinusDstAlpha" },
        { BlendFactor::SrcAlphaSaturate, "SrcAlphaSaturate" },
        { BlendFactor::ConstColor, "ConstColor" },
        { BlendFactor::OneMinusConstColor, "OneMinusConstColor" },
        { BlendFactor::Src1Color, "Src1Color" },
        { BlendFactor::OneMinusSrc1Color, "OneMinusSrc1Color" },
        { BlendFactor::Src1Alpha, "Src1Alpha" },
        { BlendFactor::OneMinusSrc1Alpha, "OneMinusSrc1Alpha" }
    };

    obj = names[type];
}

inline void from_json(const json& obj, BlendFactor& type)
{
    static std::unordered_map<std::string, BlendFactor> types {
        { "Zero", BlendFactor::Zero },
        { "One", BlendFactor::One },
        { "SrcColor", BlendFactor::SrcColor },
        { "OneMinusSrcColor", BlendFactor::OneMinusSrcColor },
        { "DstColor", BlendFactor::DstColor },
        { "OneMinusDstColor", BlendFactor::OneMinusDstColor },
        { "SrcAlpha", BlendFactor::SrcAlpha },
        { "OneMinusSrcAlpha", BlendFactor::OneMinusSrcAlpha },
        { "DstAlpha", BlendFactor::DstAlpha },
        { "OneMinusDstAlpha", BlendFactor::OneMinusDstAlpha },
        { "SrcAlphaSaturate", BlendFactor::SrcAlphaSaturate },
        { "ConstColor", BlendFactor::ConstColor },
        { "OneMinusConstColor", BlendFactor::OneMinusConstColor },
        { "Src1Color", BlendFactor::Src1Color },
        { "OneMinusSrc1Color", BlendFactor::OneMinusSrc1Color },
        { "Src1Alpha", BlendFactor::Src1Alpha },
        { "OneMinusSrc1Alpha", BlendFactor::OneMinusSrc1Alpha }
    };

    auto val = obj.get<std::string>();
    type = types[val];
}

inline void to_json(json& obj, const BlendOperation& type)
{
    static std::unordered_map<BlendOperation, std::string> names{
        { BlendOperation::Add, "Add" },
        { BlendOperation::Subtract, "Subtract" },
        { BlendOperation::ReverseSubtract, "ReverseSubtract" },
        { BlendOperation::Min, "Min" },
        { BlendOperation::Max, "Max" }
    };

    obj = names[type];
}

inline void from_json(const json& obj, BlendOperation& type)
{
    static std::unordered_map<std::string, BlendOperation> types{
        { "Add", BlendOperation::Add },
        { "Subtract", BlendOperation::Subtract },
        { "ReverseSubtract", BlendOperation::ReverseSubtract },
        { "Min", BlendOperation::Min },
        { "Max", BlendOperation::Max }
    };

    auto val = obj.get<std::string>();
    type = types[val];
}

inline void to_json(json& obj, const PixelDataFormat& type)
{
    static std::unordered_map<PixelDataFormat, std::string> names{
        { PixelDataFormat::Unspecified, "Unspecified" },
        { PixelDataFormat::Alpha8, "Alpha8" },
        { PixelDataFormat::RGB24, "RGB24" },
        { PixelDataFormat::RGBA32, "RGBA32" },
        { PixelDataFormat::RGBAFloat, "RGBAFloat" }
    };

    obj = names[type];
}

inline void from_json(const json& obj, PixelDataFormat& type)
{
    static std::unordered_map<std::string, PixelDataFormat> types{
        { "Unspecified", PixelDataFormat::Unspecified },
        { "Alpha8", PixelDataFormat::Alpha8 },
        { "RGB24", PixelDataFormat::RGB24 },
        { "RGBA32", PixelDataFormat::RGBA32 },
        { "RGBAFloat", PixelDataFormat::RGBAFloat }
    };

    auto val = obj.get<std::string>();
    type = types[val];
}


}
}
