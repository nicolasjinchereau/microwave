/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#import <UIKit/UIKit.h>
#include <MW/Math/Math.h>
#include <MW/System/WindowIOS.h>
#include <MW/Graphics/WindowSurface.h>
#include <MW/Graphics/Internal/GraphicsContextIOSOpenGL.h>
#include <MW/Graphics/Internal/OpenGLAPI.h>

namespace mw::gfx {

class WindowSurfaceIOSOpenGL : public WindowSurface
{
public:
    sptr<GraphicsContextIOSOpenGL> graphics;
    sptr<WindowIOS> window;
    
    int depthBits = 0;
    GLuint frameBuffer = 0;
    GLuint colorBuffer = 0;
    GLuint depthBuffer = 0;
    
    WindowSurfaceIOSOpenGL(
        const sptr<GraphicsContextIOSOpenGL>& graphics,
        const sptr<WindowIOS>& window);

    ~WindowSurfaceIOSOpenGL();

    virtual void UpdateSize() override;
    
    virtual void Bind(const sptr<GraphicsContext>& graphics) override;
    virtual void AllocateGraphicsResource() override;
    virtual void ReleaseGraphicsResource() override;
    virtual bool IsGraphicsResourceAllocated() const override;
    virtual GraphicsDriverType GetGraphicsDriverType() const override;
};

}
