/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once

#include <MW/System/Internal/Platform.h>

#if PLATFORM_WINDOWS
#  include <Windows.h>
#  include <windowsx.h>
#  include <wrl/client.h>
#  include <d3d11_1.h>
#  include <d3dcompiler.h>
#  include <DirectXMath.h>
#  include <glew.h>
#  include <gl/GL.h>
#elif PLATFORM_MACOS
#  import <OpenGL/OpenGL.h>
#  import <OpenGL/gl3.h>
#  import <OpenGL/gl3ext.h>
#elif PLATFORM_IOS
#  import <OpenGLES/ES2/gl.h>
#  import <OpenGLES/ES2/glext.h>
#elif PLATFORM_ANDROID
#  include <GLES2/gl2.h>
#  include <GLES2/gl2ext.h>
#  include <EGL/egl.h>
#  include <EGL/eglext.h>
#endif
