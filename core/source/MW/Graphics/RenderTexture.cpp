/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.RenderTexture;
import Microwave.Graphics.Color32;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.Internal.HWRenderTarget;
import Microwave.Graphics.Internal.HWRenderTexture;
import <vector>;
import <span>;
import <cstddef>;

namespace mw {
inline namespace gfx {

RenderTexture::RenderTexture(const IVec2& size)
{
    auto graphics = GraphicsContext::GetCurrent();
    if (!graphics)
        throw std::runtime_error("no active graphics context");

    std::vector<Color32> pixels;
    pixels.resize(size.x * size.y);
    auto data = std::as_writable_bytes(std::span(pixels));

    auto tex = graphics->context->CreateTexture(size, PixelDataFormat::RGBA32, true, data);
    tex->SetWrapMode(TextureWrapMode::Clamp);
    tex->SetFilterMode(TextureFilterMode::Bilinear);
    
    renderTexture = graphics->context->CreateRenderTexture(tex);
}

sptr<HWRenderTarget> RenderTexture::GetHWRenderTarget() {
    return renderTexture;
}

} // gfx
} // mw
