/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.RenderTextureOpenGL;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.RenderTexture;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.Math;
import Microwave.System.Pointers;
import <MW/System/Internal/PlatformHeaders.h>;

export namespace mw {
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

} // gfx
} // mw
