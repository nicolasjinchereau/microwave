/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/RenderTexture.h>
#include <MW/Graphics/Internal/OpenGLAPI.h>

namespace mw {
inline namespace gfx {

class RenderTextureOpenGL : public RenderTexture
{
public:
    RenderTextureFormat _format = RenderTextureFormat::RGBA32;
    GLuint frameBuffer = 0;
    GLuint depthBuffer = 0;
    int depthBits = 0;
    IVec2 size;

    RenderTextureOpenGL(const Vec2& size);

    virtual void Bind(const sptr<GraphicsContext>& graphics) override;
    virtual void AllocateGraphicsResource() override;
    virtual void ReleaseGraphicsResource() override;
    virtual bool IsGraphicsResourceAllocated() const override;
    virtual GraphicsDriverType GetGraphicsDriverType() const override;
    virtual IVec2 GetSize() override;
};

}
}
