/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#import <AppKit/AppKit.h>
#include <MW/Math/Math.h>
#include <MW/System/WindowMacOS.h>
#include <MW/Graphics/WindowSurface.h>
#include <MW/Graphics/Internal/GraphicsContextMacOSOpenGL.h>

namespace mw::gfx {

class WindowSurfaceMacOSOpenGL : public WindowSurface
{
public:
    sptr<GraphicsContextMacOSOpenGL> graphics;
    sptr<WindowMacOS> window;
    
    IVec2 size;
    int depthBits = 0;

    WindowSurfaceMacOSOpenGL(
        const sptr<GraphicsContextMacOSOpenGL>& graphics,
        const sptr<WindowMacOS>& window);

    ~WindowSurfaceMacOSOpenGL();

    virtual void Bind(const sptr<GraphicsContext>& graphics) override;
    virtual void AllocateGraphicsResource() override;
    virtual void ReleaseGraphicsResource() override;
    virtual bool IsGraphicsResourceAllocated() const override;
    virtual GraphicsDriverType GetGraphicsDriverType() const override;
    virtual void UpdateSize() override;
};

}
