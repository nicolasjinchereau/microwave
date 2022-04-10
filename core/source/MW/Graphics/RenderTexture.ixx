/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.RenderTexture;
import Microwave.Graphics.RenderTarget;

export namespace mw {
inline namespace gfx {

enum class RenderTextureFormat : int
{
    Alpha8,
    RGB24,
    RGBA32,
};

class RenderTexture : public RenderTarget
{
public:
    virtual ~RenderTexture(){};

};

} // gfx
} // mw
