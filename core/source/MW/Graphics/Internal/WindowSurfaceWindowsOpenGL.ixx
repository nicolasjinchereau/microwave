/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.WindowSurfaceWindowsOpenGL;
import Microwave.Graphics.WindowSurface;
import Microwave.Graphics.Internal.GraphicsContextWindowsOpenGL;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.Math;
import Microwave.System.Internal.WindowWindows;
import Microwave.System.Pointers;
import <MW/System/Internal/PlatformHeaders.h>;

export namespace mw {
inline namespace gfx {

class WindowSurfaceWindowsOpenGL : public WindowSurface
{
public:
    sptr<GraphicsContextWindowsOpenGL> graphics;
    sptr<WindowWindows> window;

    GLuint frameBuffer = 0;
    GLuint depthBuffer = 0;
    IVec2 size;
    int depthBits = 0;

    WindowSurfaceWindowsOpenGL(
        const sptr<GraphicsContextWindowsOpenGL>& graphics,
        const sptr<WindowWindows>& window);

    ~WindowSurfaceWindowsOpenGL();

    virtual void Bind(const sptr<GraphicsContext>& graphics) override;
    virtual void AllocateGraphicsResource() override;
    virtual void ReleaseGraphicsResource() override;
    virtual bool IsGraphicsResourceAllocated() const override;
    virtual GraphicsDriverType GetGraphicsDriverType() const override;
    virtual void UpdateSize() override;
    virtual IVec2 GetSize() override;
};

}
}
