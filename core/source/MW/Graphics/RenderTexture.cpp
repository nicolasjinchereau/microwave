/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.RenderTexture;
import Microwave.Graphics.Color32;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.Internal.HWRenderTarget;
import Microwave.Graphics.Internal.HWRenderTexture;
import Microwave.Graphics.Internal.HWTexture;
import Microwave.System.Exception;
import std;

namespace mw {
inline namespace gfx {

RenderTexture::RenderTexture(const IVec2& size)
    : Texture(PixelDataFormat::RGBA32, size, true)
{
    auto graphics = GraphicsContext::GetCurrent();
    if (!graphics)
        throw Exception("no active graphics context");

    renderTexture = graphics->context->CreateRenderTexture(GetHWTexture());
}

gptr<HWRenderTarget> RenderTexture::GetHWRenderTarget() {
    return renderTexture;
}

IVec2 RenderTexture::GetSize() const {
    return renderTexture->GetSize();
}


} // gfx
} // mw
