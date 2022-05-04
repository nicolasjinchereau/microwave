/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#include <MW/System/Internal/Platform.h>

module Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.Internal.HWContextD3D11;
import Microwave.Graphics.Internal.HWContextOpenGL;
import Microwave.Graphics;
import Microwave.System.Dispatcher;
import Microwave.System.Pointers;
import <stdexcept>;

namespace mw {
inline namespace gfx {

thread_local sptr<GraphicsContext> GraphicsContext::currentContext = {};

void GraphicsContext::SetCurrent(const sptr<GraphicsContext>& current)
{
    if(currentContext)
        currentContext->context->SetActive(false);

    currentContext = current;

    if(currentContext)
        currentContext->context->SetActive(true);
}

sptr<GraphicsContext> GraphicsContext::GetCurrent() {
    return currentContext;
}

GraphicsContext::GraphicsContext(GraphicsDriverType type)
{
    if(type == GraphicsDriverType::Default)
    {
#if PLATFORM_WINDOWS
        context = spnew<HWContextD3D11>();
#elif PLATFORM_IOS || PLATFORM_ANDROID || PLATFORM_MACOS
        context = spnew<HWContextOpenGL>();
#endif
    }
    else if(type == GraphicsDriverType::Direct3D11)
    {
#if PLATFORM_WINDOWS
        context = spnew<HWContextD3D11>();
#endif
    }
    else if(type == GraphicsDriverType::OpenGL)
    {
#if PLATFORM_WINDOWS || PLATFORM_IOS || PLATFORM_ANDROID || PLATFORM_MACOS
        context = spnew<HWContextOpenGL>();
#endif
    }

    if(!context)
        throw std::runtime_error("requested driver is not available");

    context->SetDepthTest(depthTest);
    context->SetDepthWriteEnabled(depthWriteEnabled);
    context->SetScissorTestEnabled(scissorTestEnabled);
    context->SetBlendingEnabled(blendingEnabled);
    context->SetBlendFactors(srcColorFactor, dstColorFactor, srcAlphaFactor, dstAlphaFactor);
    context->SetBlendOperations(colorBlendOperation, alphaBlendOperation);
    context->SetCullMode(cullMode);
    context->SetClearColor(clearColor);
    context->SetColorMask(colorMask[0], colorMask[1], colorMask[2], colorMask[3]);
    context->SetViewport(viewportRect);
    context->SetScissorRect(scissorRect);
    context->SetSwapInterval(swapInterval);
}

GraphicsContext::~GraphicsContext()
{
    renderTarget = nullptr;
    context = nullptr;
}

Vec2 GraphicsContext::GetDepthRangeNDC() const {
    return context->GetDepthRangeNDC();
}

IntRect GraphicsContext::GetViewport() const {
    return viewportRect;
}

void GraphicsContext::SetViewport(const IntRect& rect)
{
    viewportRect = rect;
    context->SetViewport(rect);
}

bool GraphicsContext::IsBlendingEnabled() const {
    return blendingEnabled;
}

void GraphicsContext::SetBlendingEnabled(bool enabled) {
    if (blendingEnabled != enabled) {
        blendingEnabled = enabled;
        context->SetBlendingEnabled(enabled);
    }
}

bool GraphicsContext::IsScissorTestEnabled() const {
    return scissorTestEnabled;
}

void GraphicsContext::SetScissorTestEnabled(bool enabled) {
    if (scissorTestEnabled != enabled) {
        scissorTestEnabled = enabled;
        context->SetScissorTestEnabled(enabled);
    }
}

IntRect GraphicsContext::GetScissorRect() const {
    return scissorRect;
}

void GraphicsContext::SetScissorRect(const IntRect& rect)
{
    if (scissorRect != rect) {
        scissorRect = rect;
        context->SetScissorRect(rect);
    }
}

Color GraphicsContext::GetClearColor() const {
    return clearColor;
}

void GraphicsContext::SetClearColor(const Color& color) {
    if(clearColor != color) {
        clearColor = color;
        context->SetClearColor(color);
    }
}

void GraphicsContext::SetColorMask(bool red, bool green, bool blue, bool alpha)
{
    if (colorMask[0] != red ||
        colorMask[1] != green ||
        colorMask[2] != blue ||
        colorMask[3] != alpha)
    {
        colorMask[0] = red;
        colorMask[1] = green;
        colorMask[2] = blue;
        colorMask[3] = alpha;
        context->SetColorMask(red, green, blue, alpha);
    }
}

void GraphicsContext::SetBlendFactors(BlendFactor source, BlendFactor dest)
{
    if (srcColorFactor != source ||
        dstColorFactor != dest ||
        srcAlphaFactor != source ||
        dstAlphaFactor != dest)
    {
        srcColorFactor = source;
        dstColorFactor = dest;
        srcAlphaFactor = source;
        dstAlphaFactor = dest;
        context->SetBlendFactors(source, dest, source, dest);
    }
}

void GraphicsContext::SetBlendFactors(
    BlendFactor sourceColor, BlendFactor destColor,
    BlendFactor sourceAlpha, BlendFactor destAlpha)
{
    if (srcColorFactor != sourceColor ||
        dstColorFactor != destColor ||
        srcAlphaFactor != sourceAlpha ||
        dstAlphaFactor != destAlpha)
    {
        srcColorFactor = sourceColor;
        dstColorFactor = destColor;
        srcAlphaFactor = sourceAlpha;
        dstAlphaFactor = destAlpha;
        context->SetBlendFactors(sourceColor, destColor, sourceAlpha, destAlpha);
    }
}

BlendFactor GraphicsContext::GetSourceColorBlendFactor() const {
    return srcColorFactor;
}

BlendFactor GraphicsContext::GetDestColorBlendFactor() const {
    return dstColorFactor;
}

BlendFactor GraphicsContext::GetSourceAlphaBlendFactor() const {
    return srcAlphaFactor;
}

BlendFactor GraphicsContext::GetDestAlphaBlendFactor() const {
    return dstAlphaFactor;
}

void GraphicsContext::SetBlendOperations(BlendOperation blendOp)
{
    if (colorBlendOperation != blendOp || alphaBlendOperation != blendOp)
    {
        colorBlendOperation = blendOp;
        alphaBlendOperation = blendOp;
        context->SetBlendOperations(blendOp, blendOp);
    }
}

void GraphicsContext::SetBlendOperations(
    BlendOperation colorBlendOp, BlendOperation alphaBlendOp)
{
    if (colorBlendOperation != colorBlendOp || alphaBlendOperation != alphaBlendOp)
    {
        colorBlendOperation = colorBlendOp;
        alphaBlendOperation = alphaBlendOp;
        context->SetBlendOperations(colorBlendOp, alphaBlendOp);
    }
}

BlendOperation GraphicsContext::GetColorBlendOperation() const {
    return colorBlendOperation;
}

BlendOperation GraphicsContext::GetAlphaBlendOperation() const {
    return alphaBlendOperation;
}

Color GraphicsContext::GetBlendColor() const {
    return blendColor;
}

void GraphicsContext::SetBlendColor(Color color)
{
    if (blendColor != color) {
        blendColor = color;
        context->SetBlendColor(color);
    }
}

CullMode GraphicsContext::GetCullMode() const {
    return cullMode;
}

void GraphicsContext::SetCullMode(CullMode mode)
{
    if (cullMode != mode) {
        cullMode = mode;
        context->SetCullMode(mode);
    }
}

DepthTest GraphicsContext::GetDepthTest() const {
    return depthTest;
}

void GraphicsContext::SetDepthTest(DepthTest test) {
    depthTest = test;
    context->SetDepthTest(test);
}

bool GraphicsContext::IsDepthWriteEnabled() const {
    return depthWriteEnabled;
}

void GraphicsContext::SetDepthWriteEnabled(bool enabled) {
    if(depthWriteEnabled != enabled) {
        depthWriteEnabled = enabled;
        context->SetDepthWriteEnabled(enabled);
    }
}

int GraphicsContext::GetSwapInterval() const {
    return swapInterval;
}

void GraphicsContext::SetSwapInterval(int interval) {
    if (swapInterval != interval) {
        swapInterval = interval;
        context->SetSwapInterval(interval);
    }
}

sptr<RenderTarget> GraphicsContext::GetRenderTarget() const {
    return renderTarget;
}

void GraphicsContext::SetRenderTarget(const sptr<RenderTarget>& target)
{
    if(renderTarget != target)
    {
        renderTarget = target;
        context->SetRenderTarget(renderTarget->GetHWRenderTarget());
        auto rc = IntRect(renderTarget ? renderTarget->GetSize() : IVec2());
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContext::RebindRenderTarget()
{
    context->SetRenderTarget(renderTarget->GetHWRenderTarget());
    auto rc = IntRect(renderTarget ? renderTarget->GetSize() : IVec2());
    SetViewport(rc);
    SetScissorRect(rc);
}

void GraphicsContext::Clear(bool depth, bool color) {
    context->Clear(renderTarget->GetHWRenderTarget(), depth, color);
}

void GraphicsContext::Flush() {
    context->Flush();
}

void GraphicsContext::Flip() {
    context->Flip(renderTarget->GetHWRenderTarget());
}

void GraphicsContext::DrawArray(int start, int count, DrawMode mode) {
    context->DrawArray(start, count, mode);
}

void GraphicsContext::DrawIndexed(int start, int count, DrawMode mode) {
    context->DrawIndexed(start, count, mode);
}

Mat4 GraphicsContext::GetOrthoMatrix(
    float left, float right,
    float bottom, float top,
    float znear, float zfar)
{
    return context->GetOrthoMatrix(left, right, bottom, top, znear, zfar);
}

Mat4 GraphicsContext::GetPerspectiveMatrix(
    float fovY, float aspect,
    float znear, float zfar)
{
    return context->GetPerspectiveMatrix(fovY, aspect, znear, zfar);
}

} // gfx
} // mw
