/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWTexture;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Math;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <cstddef>;
import <cstdint>;
import <span>;
import <string>;

export namespace mw {
inline namespace gfx {

class HWTexture : public Object
{
public:
    virtual ~HWTexture(){}
    virtual void SetPixels(const std::span<std::byte>& data, const IntRect& rect) = 0;
    virtual void SetWrapMode(TextureWrapMode mode) = 0;
    virtual void SetFilterMode(TextureFilterMode mode) = 0;
    virtual void SetAnisoLevel(float level) = 0;
};

} // gfx
} // mw
