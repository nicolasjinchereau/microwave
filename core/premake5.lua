require "android-lib"

workspace "microwave"
    configurations { "Debug", "Release" }
    location ("projects/" .. os.target())

project "microwave"
    uuid "F21BE648-5E86-9ABF-A7C4-4B65136E7814"
    kind "StaticLib"
    language "C++"
    
    location ("projects/" .. os.target())
    targetname "microwave"

    sysincludedirs {
        "source",
        "../third_party/bullet/src",
        "../third_party/dr-mp3/include",
        "../third_party/freetype2/include",
        "../third_party/GLEW/include",
        "../third_party/hlslparser/src",
        "../third_party/libjpeg/source",
        "../third_party/libpng/source",
        "../third_party/libzip/src",
        "../third_party/ogg/include",
        "../third_party/openal-soft/include",
        "../third_party/tinyexr/include",
        "../third_party/tinyxml2/src",
        "../third_party/utfcpp-3.1",
        "../third_party/vorbis/include",
        "../third_party/xz-utils/src/liblzma/api"
    }

    files {
        "source/MW/Audio/AudioStream.h",
        "source/MW/Audio/Mp3Decoder.h",
        "source/MW/Audio/Mp3Stream.cpp",
        "source/MW/Audio/Mp3Stream.h",
        "source/MW/Audio/OggDecoder.h",
        "source/MW/Audio/OggStream.cpp",
        "source/MW/Audio/OggStream.h",
        "source/MW/Audio/WavStream.cpp",
        "source/MW/Audio/WavStream.h",
        "source/MW/Audio/AudioClip.cpp",
        "source/MW/Audio/AudioClip.h",
        "source/MW/Audio/AudioContext.cpp",
        "source/MW/Audio/AudioContext.h",
        "source/MW/Audio/AudioContextWindows.cpp",
        "source/MW/Audio/AudioContextWindows.h",
        "source/MW/Audio/AudioSample.h",
        "source/MW/Data/Library/BinaryLoader.h",
        "source/MW/Data/Library/FontLoader.h",
        "source/MW/Data/Library/ObjectLoader.h",
        "source/MW/Data/Library/ShaderLoader.cpp",
        "source/MW/Data/Library/ShaderLoader.h",
        "source/MW/Data/Library/TextureLoader.cpp",
        "source/MW/Data/Library/TextureLoader.h",
        "source/MW/Data/Library/AssetLibrary.cpp",
        "source/MW/Data/Library/AssetLibrary.h",
        "source/MW/Data/Library/AssetLoader.h",
        "source/MW/Data/Library/AssetManifest.h",
        "source/MW/Data/Library/AssetSettings.h",
        "source/MW/Data/Library/AssetType.h",
        "source/MW/Data/Library/AudioClipLoader.cpp",
        "source/MW/Data/Library/AudioClipLoader.h",
        "source/MW/Graphics/detail/Color32F.h",
        "source/MW/Graphics/detail/Color32F.inl",
        "source/MW/Graphics/detail/ColorBGRA.h",
        "source/MW/Graphics/detail/ColorBGRA.inl",
        "source/MW/Graphics/detail/Color8U.h",
        "source/MW/Graphics/detail/Color8U.inl",
        "source/MW/Graphics/LineEnumerator.h",
        "source/MW/Graphics/LineEnumerator.cpp",
        "source/MW/Graphics/Font.cpp",
        "source/MW/Graphics/Font.h",
        "source/MW/Graphics/GizmoUtility.cpp",
        "source/MW/Graphics/GizmoUtility.h",
        "source/MW/Graphics/Graphics.h",
        "source/MW/Graphics/GraphicsContext.h",
        "source/MW/Graphics/GraphicsResource.h",
        "source/MW/Graphics/GraphicsTypes.h",
        "source/MW/Graphics/Image.cpp",
        "source/MW/Graphics/Image.h",
        "source/MW/Graphics/Material.cpp",
        "source/MW/Graphics/Material.h",
        "source/MW/Graphics/MaterialPropertyBlock.cpp",
        "source/MW/Graphics/MaterialPropertyBlock.h",
        "source/MW/Graphics/Mesh.cpp",
        "source/MW/Graphics/Mesh.h",
        "source/MW/Graphics/Model.cpp",
        "source/MW/Graphics/Model.h",
        "source/MW/Graphics/RenderQueue.h",
        "source/MW/Graphics/RenderTarget.h",
        "source/MW/Graphics/RenderTexture.h",
        "source/MW/Graphics/Shader.cpp",
        "source/MW/Graphics/Shader.h",
        "source/MW/Graphics/Texture.h",
        "source/MW/Graphics/Types.h",
        "source/MW/Graphics/WindowSurface.h",
        "source/MW/Graphics/AnimationClip.cpp",
        "source/MW/Graphics/AnimationClip.h",
        "source/MW/Graphics/AnimationTrack.h",
        "source/MW/Graphics/Color.h",
        "source/MW/Graphics/DrawBuffer.h",
        "source/MW/IO/FileStream.h",
        "source/MW/IO/MemoryStream.h",
        "source/MW/IO/Stream.h",
        "source/MW/IO/File.cpp",
        "source/MW/IO/File.h",
        "source/MW/Math/detail/IntRect.inl",
        "source/MW/Math/detail/IVec2.h",
        "source/MW/Math/detail/IVec2.inl",
        "source/MW/Math/detail/IVec4.h",
        "source/MW/Math/detail/IVec4.inl",
        "source/MW/Math/detail/Json.inl",
        "source/MW/Math/detail/Mat2.h",
        "source/MW/Math/detail/Mat2.inl",
        "source/MW/Math/detail/Mat3.h",
        "source/MW/Math/detail/Mat3.inl",
        "source/MW/Math/detail/Mat4.h",
        "source/MW/Math/detail/Mat4.inl",
        "source/MW/Math/detail/Plane.h",
        "source/MW/Math/detail/Plane.inl",
        "source/MW/Math/detail/Quat.h",
        "source/MW/Math/detail/Quat.inl",
        "source/MW/Math/detail/Ray.h",
        "source/MW/Math/detail/Ray.inl",
        "source/MW/Math/detail/Sphere.h",
        "source/MW/Math/detail/Sphere.inl",
        "source/MW/Math/detail/Transform.h",
        "source/MW/Math/detail/Transform.inl",
        "source/MW/Math/detail/Triangle.h",
        "source/MW/Math/detail/Triangle.inl",
        "source/MW/Math/detail/Vec2.h",
        "source/MW/Math/detail/Vec2.inl",
        "source/MW/Math/detail/Vec3.h",
        "source/MW/Math/detail/Vec3.inl",
        "source/MW/Math/detail/Vec4.h",
        "source/MW/Math/detail/Vec4.inl",
        "source/MW/Math/detail/AABox.h",
        "source/MW/Math/detail/AABox.inl",
        "source/MW/Math/detail/Functions.h",
        "source/MW/Math/detail/Functions.inl",
        "source/MW/Math/detail/IntRect.h",
        "source/MW/Math/Math.h",
        "source/MW/Scene/Components/Button.cpp",
        "source/MW/Scene/Components/Button.h",
        "source/MW/Scene/Components/Camera.cpp",
        "source/MW/Scene/Components/Camera.h",
        "source/MW/Scene/Components/Canvas.cpp",
        "source/MW/Scene/Components/Canvas.h",
        "source/MW/Scene/Components/CapsuleCollider.cpp",
        "source/MW/Scene/Components/CapsuleCollider.h",
        "source/MW/Scene/Components/Collider.cpp",
        "source/MW/Scene/Components/Collider.h",
        "source/MW/Scene/Components/Component.cpp",
        "source/MW/Scene/Components/Component.h",
        "source/MW/Scene/Components/D6Joint.cpp",
        "source/MW/Scene/Components/D6Joint.h",
        "source/MW/Scene/Components/DirectionalLight.cpp",
        "source/MW/Scene/Components/DirectionalLight.h",
        "source/MW/Scene/Components/ImageView.cpp",
        "source/MW/Scene/Components/ImageView.h",
        "source/MW/Scene/Components/MeshCollider.cpp",
        "source/MW/Scene/Components/MeshCollider.h",
        "source/MW/Scene/Components/MeshRenderer.cpp",
        "source/MW/Scene/Components/MeshRenderer.h",
        "source/MW/Scene/Components/RigidBody.cpp",
        "source/MW/Scene/Components/RigidBody.h",
        "source/MW/Scene/Components/Script.h",
        "source/MW/Scene/Components/SphereCollider.cpp",
        "source/MW/Scene/Components/SphereCollider.h",
        "source/MW/Scene/Components/TextView.cpp",
        "source/MW/Scene/Components/TextView.h",
        "source/MW/Scene/Components/View.cpp",
        "source/MW/Scene/Components/View.h",
        "source/MW/Scene/Components/Animator.cpp",
        "source/MW/Scene/Components/Animator.h",
        "source/MW/Scene/Components/AudioSource.cpp",
        "source/MW/Scene/Components/AudioSource.h",
        "source/MW/Scene/Components/BoxCollider.cpp",
        "source/MW/Scene/Components/BoxCollider.h",
        "source/MW/Scene/Internal/CapsuleShape.h",
        "source/MW/Scene/Internal/Bullet.h",
        "source/MW/Scene/Node.h",
        "source/MW/Scene/PhysicsWorld.cpp",
        "source/MW/Scene/PhysicsWorld.h",
        "source/MW/Scene/Renderable.h",
        "source/MW/Scene/Scene.cpp",
        "source/MW/Scene/Scene.h",
        "source/MW/Scene/SceneRenderer.cpp",
        "source/MW/Scene/SceneRenderer.h",
        "source/MW/Scene/Axis.h",
        "source/MW/Scene/Coroutine.h",
        "source/MW/Scene/Events.cpp",
        "source/MW/Scene/Events.h",
        "source/MW/Scene/LayerMask.h",
        "source/MW/Scene/Node.cpp",
        "source/MW/System/EntryPoint.cpp",
        "source/MW/System/EventHandlerList.h",
        "source/MW/System/Executor.h",
        "source/MW/System/IAwaitable.h",
        "source/MW/System/IAwaiter.h",
        "source/MW/System/Json.h",
        "source/MW/System/Object.cpp",
        "source/MW/System/Object.h",
        "source/MW/System/path.h",
        "source/MW/System/Platforms.h",
        "source/MW/System/Pointers.h",
        "source/MW/System/Spinlock.h",
        "source/MW/System/SyncExecutor.h",
        "source/MW/System/Task.cpp",
        "source/MW/System/Task.h",
        "source/MW/System/ThreadPool.h",
        "source/MW/System/UUID.h",
        "source/MW/System/Window.cpp",
        "source/MW/System/Window.h",
        "source/MW/System/App.cpp",
        "source/MW/System/App.h",
        "source/MW/System/ApplicationDispatcher.h",
        "source/MW/System/AsyncExecutor.cpp",
        "source/MW/System/AsyncExecutor.h",
        "source/MW/System/Awaitable.h",
        "source/MW/System/Clock.h",
        "source/MW/System/Console.cpp",
        "source/MW/System/Console.h",
        "source/MW/System/Dispatcher.cpp",
        "source/MW/System/Dispatcher.h",
        "source/MW/Utilities/BinPacking/BSPNodeAllocator.cpp",
        "source/MW/Utilities/BinPacking/BSPNodeAllocator.h",
        "source/MW/Utilities/BinPacking/RectMapping.h",
        "source/MW/Utilities/BinPacking/Bin.h",
        "source/MW/Utilities/BinPacking/BinPacking.cpp",
        "source/MW/Utilities/BinPacking/BinPacking.h",
        "source/MW/Utilities/BinPacking/BSPNode.cpp",
        "source/MW/Utilities/BinPacking/BSPNode.h",
        "source/MW/Utilities/Format.h",
        "source/MW/Utilities/Sink.h",
        "source/MW/Utilities/Util.h",
        "source/MW/Utilities/Base64.h",
        "source/MW/Utilities/EnumFlags.h"
    }

    androidabis {
		"armeabi-v7a", "arm64-v8a"
    }

    androidminsdkversion "android-16"

    filter { "system:windows" }
        sysincludedirs {
            "../third_party/fbx/include"
        }
        files {
            "source/MW/Data/Database/AssetDatabase.cpp",
            "source/MW/Data/Database/AssetDatabase.h",
            "source/MW/Data/Database/AssetImporter.h",
            "source/MW/Data/Database/AudioClipImporter.h",
            "source/MW/Data/Database/DefaultImporter.h",
            "source/MW/Data/Database/ModelImporter.cpp",
            "source/MW/Data/Database/ModelImporter.h",
            "source/MW/Data/Database/TextureImporter.h",
            "source/MW/Data/Internal/FbxMemoryStream.h",
            "source/MW/Data/Internal/FBXModelConverter.h",
            "source/MW/Data/Internal/FBXUDPParser.h",
            "source/MW/Graphics/Internal/DrawBufferOpenGL.cpp",
            "source/MW/Graphics/Internal/DrawBufferOpenGL.h",
            "source/MW/Graphics/Internal/DrawBufferWindowsD3D11.cpp",
            "source/MW/Graphics/Internal/DrawBufferWindowsD3D11.h",
            "source/MW/Graphics/Internal/GraphicsContextWindows.cpp",
            "source/MW/Graphics/Internal/GraphicsContextWindowsD3D11.cpp",
            "source/MW/Graphics/Internal/GraphicsContextWindowsD3D11.h",
            "source/MW/Graphics/Internal/GraphicsContextWindowsOpenGL.cpp",
            "source/MW/Graphics/Internal/GraphicsContextWindowsOpenGL.h",
            "source/MW/Graphics/Internal/OpenGLAPI.h",
            "source/MW/Graphics/Internal/RenderTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/RenderTextureOpenGL.h",
            "source/MW/Graphics/Internal/RenderTextureWindowsD3D11.cpp",
            "source/MW/Graphics/Internal/RenderTextureWindowsD3D11.h",
            "source/MW/Graphics/Internal/ShaderOpenGL.cpp",
            "source/MW/Graphics/Internal/ShaderOpenGL.h",
            "source/MW/Graphics/Internal/ShaderWindowsD3D11.cpp",
            "source/MW/Graphics/Internal/ShaderWindowsD3D11.h",
            "source/MW/Graphics/Internal/TextureOpenGL.cpp",
            "source/MW/Graphics/Internal/TextureOpenGL.h",
            "source/MW/Graphics/Internal/TextureWindowsD3D11.cpp",
            "source/MW/Graphics/Internal/TextureWindowsD3D11.h",
            "source/MW/Graphics/Internal/WindowSurfaceWindowsD3D11.cpp",
            "source/MW/Graphics/Internal/WindowSurfaceWindowsD3D11.h",
            "source/MW/Graphics/Internal/WindowSurfaceWindowsOpenGL.cpp",
            "source/MW/Graphics/Internal/WindowSurfaceWindowsOpenGL.h",
            "source/MW/System/Internal/ApplicationDispatcherWindows.cpp",
            "source/MW/System/Internal/ApplicationDispatcherWindows.h",
            "source/MW/System/Internal/WindowWindows.cpp",
            "source/MW/System/Internal/WindowWindows.h",
        }

    filter { "system:macosx" }
        files {
            "source/MW/Graphics/Internal/DrawBufferOpenGL.cpp",
            "source/MW/Graphics/Internal/DrawBufferOpenGL.h",
            "source/MW/Graphics/Internal/GraphicsContextMacOS.mm",
            "source/MW/Graphics/Internal/GraphicsContextMacOSOpenGL.h",
            "source/MW/Graphics/Internal/GraphicsContextMacOSOpenGL.mm",
            "source/MW/Graphics/Internal/OpenGLAPI.h",
            "source/MW/Graphics/Internal/RenderTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/RenderTextureOpenGL.h",
            "source/MW/Graphics/Internal/ShaderOpenGL.cpp",
            "source/MW/Graphics/Internal/ShaderOpenGL.h",
            "source/MW/Graphics/Internal/TextureOpenGL.cpp",
            "source/MW/Graphics/Internal/TextureOpenGL.h",
            "source/MW/Graphics/Internal/WindowSurfaceMacOSOpenGL.h",
            "source/MW/Graphics/Internal/WindowSurfaceMacOSOpenGL.mm",
            "source/MW/System/Internal/ApplicationDispatcherMacOS.h",
            "source/MW/System/Internal/ApplicationDispatcherMacOS.mm",
            "source/MW/System/Internal/WindowMacOS.h",
            "source/MW/System/Internal/WindowMacOS.mm"
        }

    filter { "system:ios" }
        files {
            "source/MW/Graphics/Internal/DrawBufferOpenGL.cpp",
            "source/MW/Graphics/Internal/DrawBufferOpenGL.h",
            "source/MW/Graphics/Internal/GraphicsContextIOS.mm",
            "source/MW/Graphics/Internal/GraphicsContextIOSOpenGL.h",
            "source/MW/Graphics/Internal/GraphicsContextIOSOpenGL.mm",
            "source/MW/Graphics/Internal/OpenGLAPI.h",
            "source/MW/Graphics/Internal/RenderTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/RenderTextureOpenGL.h",
            "source/MW/Graphics/Internal/ShaderOpenGL.cpp",
            "source/MW/Graphics/Internal/ShaderOpenGL.h",
            "source/MW/Graphics/Internal/TextureOpenGL.cpp",
            "source/MW/Graphics/Internal/TextureOpenGL.h",
            "source/MW/Graphics/Internal/WindowSurfaceIOSOpenGL.h",
            "source/MW/Graphics/Internal/WindowSurfaceIOSOpenGL.mm",
            "source/MW/System/Internal/ApplicationDispatcherIOS.h",
            "source/MW/System/Internal/ApplicationDispatcherIOS.mm",
            "source/MW/System/Internal/WindowIOS.h",
            "source/MW/System/Internal/WindowIOS.mm"
        }

    filter { "system:android" }
        sysincludedirs {
            "$(NDK_ROOT)/sources/android/native_app_glue"
        }
        files {
            "source/MW/Graphics/Internal/DrawBufferOpenGL.cpp",
            "source/MW/Graphics/Internal/DrawBufferOpenGL.h",
            "source/MW/Graphics/Internal/OpenGLAPI.h",
            "source/MW/Graphics/Internal/RenderTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/RenderTextureOpenGL.h",
            "source/MW/Graphics/Internal/ShaderOpenGL.cpp",
            "source/MW/Graphics/Internal/ShaderOpenGL.h",
            "source/MW/Graphics/Internal/TextureOpenGL.cpp",
            "source/MW/Graphics/Internal/TextureOpenGL.h",
            "source/MW/Graphics/Internal/WindowSurfaceAndroidOpenGL.cpp",
            "source/MW/Graphics/Internal/WindowSurfaceAndroidOpenGL.h",
            "source/MW/IO/AndroidAssetStream.h",
            "source/MW/System/Internal/ApplicationDispatcherAndroid.cpp",
            "source/MW/System/Internal/ApplicationDispatcherAndroid.h",
            "source/MW/System/Internal/WindowAndroid.cpp",
            "source/MW/System/Internal/WindowAndroid.h"
        }

    filter { "action:vs*" }
        objdir ("!obj/$(PlatformName)/$(Configuration)")
        targetdir ("lib/$(PlatformName)/$(Configuration)")
        characterset "MBCS"

    filter { "action:xcode*" }
        objdir ("!obj/$(PLATFORM_NAME)/$(CONFIGURATION)")
        targetdir ("lib/$(PLATFORM_NAME)/$(CONFIGURATION)")

    filter { "action:android*" }
        objdir ("obj/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)")
        targetdir ("lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)")

    filter "configurations:Debug"
        defines {
            "_DEBUG",
            "NOMINMAX",
            "FBXSDK_SHARED",
            "AL_ALEXT_PROTOTYPES"
        }
        symbols "On"

    filter "configurations:Release"
        defines {
            "NDEBUG",
            "NOMINMAX",
            "FBXSDK_SHARED",
            "AL_ALEXT_PROTOTYPES"
        }
        optimize "Speed"
        inlining "Auto"
        vectorextensions "SSE2"

    local baseSDK = ""

    if _ACTION == "xcode4" then
        if os.target() == "ios" then
            baseSDK = "iphoneos"
        elseif os.target() == "macosx" then
            baseSDK = "macosx"
        end
    end

    filter { "action:xcode*" }
        xcodebuildsettings {
            ["ARCHS"] = "$(ARCHS_STANDARD)";
            ["SDKROOT"] = baseSDK;
            ["GCC_INPUT_FILETYPE"] = "sourcecode.cpp.objcpp"; -- compile *.cpp as Objective-C++
            ["CLANG_ENABLE_OBJC_ARC"] = "YES";
            ["CLANG_CXX_LANGUAGE_STANDARD"] = "gnu++20";
            ["GCC_C_LANGUAGE_STANDARD"] = "gnu11";
            ["CLANG_CXX_LIBRARY"] = "libc++";
        }
        buildoptions { "-fcoroutines-ts" }
    
    filter { "action:vs*" }
        cppdialect "C++20"
        cdialect "C11"
        buildoptions {
            "/await"
        }
