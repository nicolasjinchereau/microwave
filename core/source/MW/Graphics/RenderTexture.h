/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/RenderTarget.h>
#include <MW/Math/Math.h>

namespace mw {
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

}
}
