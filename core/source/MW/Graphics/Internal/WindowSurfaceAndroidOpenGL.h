/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/Math.h>
#include <MW/System/WindowAndroid.h>
#include <MW/Graphics/WindowSurface.h>
#include <MW/Graphics/Internal/GraphicsContextAndroidOpenGL.h>
#include <MW/Graphics/Internal/OpenGLAPI.h>

namespace mw::gfx {

class WindowSurfaceAndroidOpenGL : public WindowSurface
{
public:
    sptr<GraphicsContextAndroidOpenGL> graphics;
    sptr<WindowAndroid> window;

    EGLSurface surface = EGL_NO_SURFACE;

    WindowSurfaceAndroidOpenGL(
        const sptr<GraphicsContextAndroidOpenGL>& graphics,
        const sptr<WindowAndroid>& window);

    ~WindowSurfaceAndroidOpenGL();

    virtual void UpdateSize() override;
    virtual void Bind(const sptr<GraphicsContext>& graphics) override;
    virtual void AllocateGraphicsResource() override;
    virtual void ReleaseGraphicsResource() override;
    virtual bool IsGraphicsResourceAllocated() const override;
    virtual GraphicsDriverType GetGraphicsDriverType() const override;
};

}
