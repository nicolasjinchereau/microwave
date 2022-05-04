/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#include <MW/System/Internal/Platform.h>

module Microwave.Graphics.Internal.HWContextOpenGL;

#if PLATFORM_WINDOWS
import Microwave.Graphics.Internal.HWDriverContextWGL;
#elif PLATFORM_ANDROID
import Microwave.Graphics.Internal.HWDriverContextEGL;
#elif PLATFORM_IOS
import Microwave.Graphics.Internal.HWDriverContextEAGL;
#elif PLATFORM_MACOS
import Microwave.Graphics.Internal.HWDriverContextNSGL;
#endif

import Microwave.Graphics.Color32;
import Microwave.Graphics.Internal.HWRenderTextureOpenGL;
import Microwave.Graphics.Internal.HWShaderOpenGL;
import Microwave.Graphics.ShaderInfo;

namespace mw {
inline namespace gfx {

std::unordered_map<BlendFactor, gl::Enum> blendFactors = {
    { BlendFactor::Zero, gl::ZERO },
    { BlendFactor::One, gl::ONE },
    { BlendFactor::SrcColor, gl::SRC_COLOR },
    { BlendFactor::OneMinusSrcColor, gl::ONE_MINUS_SRC_COLOR },
    { BlendFactor::DstColor, gl::DST_COLOR },
    { BlendFactor::OneMinusDstColor, gl::ONE_MINUS_DST_COLOR },
    { BlendFactor::SrcAlpha, gl::SRC_ALPHA },
    { BlendFactor::OneMinusSrcAlpha, gl::ONE_MINUS_SRC_ALPHA },
    { BlendFactor::DstAlpha, gl::DST_ALPHA },
    { BlendFactor::OneMinusDstAlpha, gl::ONE_MINUS_DST_ALPHA },
    { BlendFactor::SrcAlphaSaturate, gl::SRC_ALPHA_SATURATE },
    { BlendFactor::ConstColor, gl::CONSTANT_COLOR },
    { BlendFactor::OneMinusConstColor, gl::ONE_MINUS_CONSTANT_COLOR },
    { BlendFactor::Src1Color, gl::SRC1_COLOR },
    { BlendFactor::OneMinusSrc1Color, gl::ONE_MINUS_SRC1_COLOR },
    { BlendFactor::Src1Alpha, gl::SRC1_ALPHA },
    { BlendFactor::OneMinusSrc1Alpha, gl::ONE_MINUS_SRC1_ALPHA }
};

std::unordered_map<BlendOperation, gl::Enum> blendOperations = {
    { BlendOperation::Add, gl::FUNC_ADD },
    { BlendOperation::Subtract, gl::FUNC_SUBTRACT },
    { BlendOperation::ReverseSubtract, gl::FUNC_REVERSE_SUBTRACT },
    { BlendOperation::Min, gl::MIN },
    { BlendOperation::Max, gl::MAX }
};

std::unordered_map<DrawMode, gl::Enum> drawModes = {
    { DrawMode::Points, gl::POINTS },
    { DrawMode::Lines, gl::LINES },
    { DrawMode::LineStrip, gl::LINE_STRIP },
    { DrawMode::Triangles, gl::TRIANGLES },
    { DrawMode::TriangleStrip, gl::TRIANGLE_STRIP }
};

namespace detail
{
// https://www.khronos.org/opengl/wiki/Debug_Output
void GLAPIENTRY OnDebugMessage(
    gl::Enum source,
    gl::Enum type,
    gl::Uint id,
    gl::Enum severity,
    gl::Sizei length,
    const gl::Char* message,
    const void* userParam)
{
    if (severity == gl::DEBUG_SEVERITY_NOTIFICATION)
        return;

    Console::WriteLine("OpenGL: %", std::string_view(message));
}
} // detail

HWContextOpenGL::HWContextOpenGL()
{
#if PLATFORM_WINDOWS
    driverContext = spnew<HWDriverContextWGL>();
#elif PLATFORM_MACOS
    driverContext = spnew<HWDriverContextNSGL>();
#elif PLATFORM_IOS
    driverContext = spnew<HWDriverContextEAGL>();
#elif PLATFORM_ANDROID
    driverContext = spnew<HWDriverContextEGL>();
#endif

    gl::Enable(gl::DEBUG_OUTPUT);
    gl::DebugMessageCallback(detail::OnDebugMessage, nullptr);
    gl::FrontFace(gl::CW);
    gl::Disable(gl::DITHER);
}

void HWContextOpenGL::SetActive(bool active) {
    driverContext->SetActive(active);
}

Vec2 HWContextOpenGL::GetDepthRangeNDC() const {
    return Vec2(-1.0f, 1.0f);
}

void HWContextOpenGL::SetRenderTarget(const sptr<HWRenderTarget>& target) {
    driverContext->SetRenderTarget(target);
}

void HWContextOpenGL::Flip(const sptr<HWRenderTarget>& target) {
    driverContext->Flip(target);
}

void HWContextOpenGL::SetSwapInterval(int interval) {
    driverContext->SetSwapInterval(interval);
}

void HWContextOpenGL::SetCullMode(CullMode mode)
{
    switch (mode)
    {
    case CullMode::Off:
        gl::Disable(gl::CULL_FACE);
        break;

    case CullMode::Front:
        gl::Enable(gl::CULL_FACE);
        gl::CullFace(gl::FRONT);
        break;

    case CullMode::Back:
        gl::Enable(gl::CULL_FACE);
        gl::CullFace(gl::BACK);
        break;
    }
}

void HWContextOpenGL::SetScissorTestEnabled(bool enabled) {
    gl::SetEnabled(gl::SCISSOR_TEST, enabled);
}

void HWContextOpenGL::SetScissorRect(const IntRect& rect) {
    gl::Scissor(rect.x, rect.y, rect.w, rect.h);
}

void HWContextOpenGL::SetDepthTest(DepthTest test)
{
    switch (test)
    {
    default:
    case DepthTest::Always:
        gl::Disable(gl::DEPTH_TEST);
        gl::DepthFunc(gl::ALWAYS);
        break;

    case DepthTest::Never:
        gl::Enable(gl::DEPTH_TEST);
        gl::DepthFunc(gl::NEVER);
        break;

    case DepthTest::Less:
        gl::Enable(gl::DEPTH_TEST);
        gl::DepthFunc(gl::LESS);
        break;

    case DepthTest::Equal:
        gl::Enable(gl::DEPTH_TEST);
        gl::DepthFunc(gl::EQUAL);
        break;

    case DepthTest::LessOrEqual:
        gl::Enable(gl::DEPTH_TEST);
        gl::DepthFunc(gl::LEQUAL);
        break;

    case DepthTest::Greater:
        gl::Enable(gl::DEPTH_TEST);
        gl::DepthFunc(gl::GREATER);
        break;

    case DepthTest::NotEqual:
        gl::Enable(gl::DEPTH_TEST);
        gl::DepthFunc(gl::NOTEQUAL);
        break;

    case DepthTest::GreaterOrEqual:
        gl::Enable(gl::DEPTH_TEST);
        gl::DepthFunc(gl::GEQUAL);
        break;
    }
}

void HWContextOpenGL::SetDepthWriteEnabled(bool enabled) {
    depthWriteEnabled = enabled;
    gl::DepthMask(enabled);
}

void HWContextOpenGL::SetBlendingEnabled(bool enabled) {
    gl::SetEnabled(gl::BLEND, enabled);
}

void HWContextOpenGL::SetClearColor(const Color& color) {
    gl::ClearColor(color.r, color.g, color.b, color.a);
}

void HWContextOpenGL::SetBlendOperations(
    BlendOperation colorBlendOp, BlendOperation alphaBlendOp)
{
    gl::BlendEquationSeparate(
        blendOperations[colorBlendOp],
        blendOperations[alphaBlendOp]
    );
}

void HWContextOpenGL::SetBlendFactors(
    BlendFactor sourceColor, BlendFactor destColor,
    BlendFactor sourceAlpha, BlendFactor destAlpha)
{
    gl::BlendFuncSeparate(
        blendFactors[sourceColor], blendFactors[destColor],
        blendFactors[sourceAlpha], blendFactors[destAlpha]);
}

void HWContextOpenGL::SetColorMask(
    bool red,
    bool green,
    bool blue,
    bool alpha)
{
    gl::ColorMask(
        (gl::Boolean)red,
        (gl::Boolean)green,
        (gl::Boolean)blue,
        (gl::Boolean)alpha);
}

void HWContextOpenGL::SetBlendColor(Color color) {
    gl::BlendColor(color.r, color.g, color.b, color.a);
}

void HWContextOpenGL::SetViewport(const IntRect& rect) {
    gl::Viewport(rect.x, rect.y, rect.w, rect.h);
}

void HWContextOpenGL::Clear(const sptr<HWRenderTarget>& target, bool depth, bool color)
{
    if (target)
    {
        gl::Uint flags = 0;

        if (depth)
        {
            flags |= gl::DEPTH_BUFFER_BIT;

            if (!depthWriteEnabled)
                gl::DepthMask(true);
        }

        if (color)
        {
            flags |= gl::COLOR_BUFFER_BIT;
        }

        gl::Clear(flags);

        if (depth && !depthWriteEnabled)
            gl::DepthMask(false);
    }
}

void HWContextOpenGL::DrawArray(int start, int count, DrawMode mode) {
    gl::DrawArrays(drawModes[mode], start, count);
}

void HWContextOpenGL::DrawIndexed(int start, int count, DrawMode mode) {
    gl::DrawElements(drawModes[mode], count, gl::UNSIGNED_INT, nullptr);
}

void HWContextOpenGL::Flush() {
    gl::Flush();
}

Mat4 HWContextOpenGL::GetOrthoMatrix(
    float left, float right,
    float bottom, float top,
    float znear, float zfar)
{
    return Mat4::OrthoOpenGL(left, right, bottom, top, znear, zfar);
}

Mat4 HWContextOpenGL::GetPerspectiveMatrix(
    float fovY, float aspect,
    float znear, float zfar)
{
    return Mat4::PerspectiveOpenGL(fovY, aspect, znear, zfar);
}

ShaderLanguage HWContextOpenGL::GetShaderLanguage() const
{
#if PLATFORM_IOS || PLATFORM_ANDROID
    return ShaderLanguage::GLSL_100;
#elif PLATFORM_MACOS || PLATFORM_WINDOWS
    return ShaderLanguage::GLSL_150;
#endif
}

sptr<HWRenderTexture> HWContextOpenGL::CreateRenderTexture(const sptr<HWTexture>& tex) {
    return spnew<HWRenderTextureOpenGL>(tex);
}

sptr<HWShader> HWContextOpenGL::CreateShader(const sptr<ShaderInfo>& info) {
    return spnew<HWShaderOpenGL>(info);
}

sptr<HWBuffer> HWContextOpenGL::CreateBuffer(
    BufferType type, BufferUsage usage, BufferCPUAccess cpuAccess, size_t size)
{
    return spnew<HWBufferOpenGL>(type, usage, cpuAccess, size);
}

sptr<HWBuffer> HWContextOpenGL::CreateBuffer(
    BufferType type, BufferUsage usage,
    BufferCPUAccess cpuAccess, const std::span<std::byte>& data)
{
    return spnew<HWBufferOpenGL>(type, usage, cpuAccess, data);
}

sptr<HWSurface> HWContextOpenGL::CreateSurface(const sptr<Window>& window) {
    return driverContext->CreateSurface(window);
}

sptr<HWTexture> HWContextOpenGL::CreateTexture(
    const IVec2& size, PixelDataFormat format, bool dynamic,
    const std::span<std::byte>& data)
{
    return spnew<HWTextureOpenGL>(
        SharedFrom(this), size, format, dynamic, data);
}

sptr<HWTexture> HWContextOpenGL::CreateTexture(
    const IVec2& size, PixelDataFormat format, bool dynamic,
    const sptr<HWBuffer>& buffer)
{
    return spnew<HWTextureOpenGL>(
        SharedFrom(this), size, format, dynamic, buffer);
}

sptr<HWTexture> HWContextOpenGL::GetDefaultTexture()
{
    if (!defaultTexture)
    {
        IVec2 texSize = { 1, 1 };
        auto texFormat = PixelDataFormat::RGBA32;
        Color32 gray(127, 127, 127, 255);
        auto texBuff = std::as_writable_bytes(std::span<Color32>(&gray, 1));

        defaultTexture = spnew<HWTextureOpenGL>(
            SharedFrom(this), texSize, texFormat, false, texBuff);
    }

    return defaultTexture;
}

} // gfx
} // mw
