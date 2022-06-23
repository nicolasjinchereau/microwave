require "android-lib"

workspace "microwave"
    configurations { "Debug", "Release" }
    location ("projects/" .. os.target())

project "microwave"
    uuid "F21BE648-5E86-9ABF-A7C4-4B65136E7814"
    kind "StaticLib"
    language "C++"
    --architecture "x86_64"

    location ("projects/" .. os.target())
    targetname "microwave"

    sysincludedirs {
        "source",
        "../third_party/boost/include",
        "../third_party/bullet/src",
        "../third_party/dr-mp3/include",
        "../third_party/freetype2/include",
        "../third_party/GLEW/include",
        "../third_party/graph-collector/src",
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
        "source/MW/Audio/Audio.ixx",
        "source/MW/Audio/AudioClip.cpp",
        "source/MW/Audio/AudioClip.ixx",
        "source/MW/Audio/AudioContext.cpp",
        "source/MW/Audio/AudioContext.ixx",
        "source/MW/Audio/AudioSample.ixx",
        "source/MW/Audio/AudioStream.ixx",
        "source/MW/Audio/Mp3Stream.cpp",
        "source/MW/Audio/Mp3Stream.ixx",
        "source/MW/Audio/OggStream.cpp",
        "source/MW/Audio/OggStream.ixx",
        "source/MW/Audio/WavStream.cpp",
        "source/MW/Audio/WavStream.ixx",
        "source/MW/Audio/Internal/AudioContextOpenAL.cpp",
        "source/MW/Audio/Internal/AudioContextOpenAL.ixx",
        "source/MW/Audio/Internal/Mp3Decoder.ixx",
        "source/MW/Audio/Internal/OggDecoder.ixx",
        "source/MW/Audio/Internal/OpenAL.h",
        "source/MW/Data/Data.ixx",
        "source/MW/Data/Library/AssetLibrary.cpp",
        "source/MW/Data/Library/AssetLibrary.ixx",
        "source/MW/Data/Library/AssetLoader.ixx",
        "source/MW/Data/Library/AssetManifest.ixx",
        "source/MW/Data/Library/AssetSettings.ixx",
        "source/MW/Data/Library/AssetType.ixx",
        "source/MW/Data/Library/AudioClipLoader.cpp",
        "source/MW/Data/Library/AudioClipLoader.ixx",
        "source/MW/Data/Library/BinaryLoader.ixx",
        "source/MW/Data/Library/FontLoader.ixx",
        "source/MW/Data/Library/Library.ixx",
        "source/MW/Data/Library/ObjectLoader.ixx",
        "source/MW/Data/Library/ShaderLoader.cpp",
        "source/MW/Data/Library/ShaderLoader.ixx",
        "source/MW/Data/Library/TextureLoader.cpp",
        "source/MW/Data/Library/TextureLoader.ixx",
        "source/MW/Data/Internal/Assets.h",
        "source/MW/Graphics/AnimationClip.cpp",
        "source/MW/Graphics/AnimationClip.ixx",
        "source/MW/Graphics/AnimationTrack.ixx",
        "source/MW/Graphics/Buffer.cpp",
        "source/MW/Graphics/Buffer.ixx",
        "source/MW/Graphics/Color.ixx",
        "source/MW/Graphics/Color32.ixx",
        "source/MW/Graphics/ColorBGRA.ixx",
        "source/MW/Graphics/Font.cpp",
        "source/MW/Graphics/Font.ixx",
        "source/MW/Graphics/GizmoUtility.cpp",
        "source/MW/Graphics/GizmoUtility.ixx",
        "source/MW/Graphics/Graphics.ixx",
        "source/MW/Graphics/GraphicsContext.cpp",
        "source/MW/Graphics/GraphicsContext.ixx",
        "source/MW/Graphics/GraphicsTypes.ixx",
        "source/MW/Graphics/Image.cpp",
        "source/MW/Graphics/Image.ixx",
        "source/MW/Graphics/LineEnumerator.cpp",
        "source/MW/Graphics/LineEnumerator.ixx",
        "source/MW/Graphics/Material.cpp",
        "source/MW/Graphics/Material.ixx",
        "source/MW/Graphics/MaterialPropertyBlock.cpp",
        "source/MW/Graphics/MaterialPropertyBlock.ixx",
        "source/MW/Graphics/Mesh.cpp",
        "source/MW/Graphics/Mesh.ixx",
        "source/MW/Graphics/Model.cpp",
        "source/MW/Graphics/Model.ixx",
        "source/MW/Graphics/RenderQueue.ixx",
        "source/MW/Graphics/RenderTarget.ixx",
        "source/MW/Graphics/RenderTexture.cpp",
        "source/MW/Graphics/RenderTexture.ixx",
        "source/MW/Graphics/Shader.cpp",
        "source/MW/Graphics/Shader.ixx",
        "source/MW/Graphics/ShaderInfo.cpp",
        "source/MW/Graphics/ShaderInfo.ixx",
        "source/MW/Graphics/Texture.cpp",
        "source/MW/Graphics/Texture.ixx",
        "source/MW/Graphics/Types.ixx",
        "source/MW/Graphics/Internal/FreeType2.h",
        "source/MW/Graphics/Internal/HWBuffer.ixx",
        "source/MW/Graphics/Internal/HWRenderTarget.ixx",
        "source/MW/Graphics/Internal/HWRenderTexture.ixx",
        "source/MW/Graphics/Internal/HWContext.ixx",
        "source/MW/Graphics/Internal/HWShader.ixx",
        "source/MW/Graphics/Internal/HWSurface.ixx",
        "source/MW/Graphics/Internal/HWTexture.ixx",
        "source/MW/IO/File.cpp",
        "source/MW/IO/File.ixx",
        "source/MW/IO/FileStream.ixx",
        "source/MW/IO/IO.ixx",
        "source/MW/IO/MemoryStream.ixx",
        "source/MW/IO/Stream.ixx",
        "source/MW/IO/Terminal.ixx",
        "source/MW/Math/AABox.ixx",
        "source/MW/Math/Constants.ixx",
        "source/MW/Math/Functions.ixx",
        "source/MW/Math/IntRect.ixx",
        "source/MW/Math/IVec2.ixx",
        "source/MW/Math/IVec4.ixx",
        "source/MW/Math/Mat2.ixx",
        "source/MW/Math/Mat3.ixx",
        "source/MW/Math/Mat4.ixx",
        "source/MW/Math/Math.ixx",
        "source/MW/Math/MathJson.ixx",
        "source/MW/Math/Operators.ixx",
        "source/MW/Math/Plane.ixx",
        "source/MW/Math/Quat.ixx",
        "source/MW/Math/Ray.ixx",
        "source/MW/Math/Sphere.ixx",
        "source/MW/Math/Transform.ixx",
        "source/MW/Math/Triangle.ixx",
        "source/MW/Math/Vec2.ixx",
        "source/MW/Math/Vec3.ixx",
        "source/MW/Math/Vec4.ixx",
        "source/MW/SceneGraph/Components/Animator.cpp",
        "source/MW/SceneGraph/Components/Animator.ixx",
        "source/MW/SceneGraph/Components/AudioSource.cpp",
        "source/MW/SceneGraph/Components/AudioSource.ixx",
        "source/MW/SceneGraph/Components/BoxCollider.cpp",
        "source/MW/SceneGraph/Components/BoxCollider.ixx",
        "source/MW/SceneGraph/Components/Button.cpp",
        "source/MW/SceneGraph/Components/Button.ixx",
        "source/MW/SceneGraph/Components/Camera.cpp",
        "source/MW/SceneGraph/Components/Camera.ixx",
        "source/MW/SceneGraph/Components/Canvas.cpp",
        "source/MW/SceneGraph/Components/Canvas.ixx",
        "source/MW/SceneGraph/Components/CapsuleCollider.cpp",
        "source/MW/SceneGraph/Components/CapsuleCollider.ixx",
        "source/MW/SceneGraph/Components/Collider.cpp",
        "source/MW/SceneGraph/Components/Collider.ixx",
        "source/MW/SceneGraph/Components/Component.cpp",
        "source/MW/SceneGraph/Components/Component.ixx",
        "source/MW/SceneGraph/Components/Components.ixx",
        "source/MW/SceneGraph/Components/D6Joint.cpp",
        "source/MW/SceneGraph/Components/D6Joint.ixx",
        "source/MW/SceneGraph/Components/DirectionalLight.cpp",
        "source/MW/SceneGraph/Components/DirectionalLight.ixx",
        "source/MW/SceneGraph/Components/ImageView.cpp",
        "source/MW/SceneGraph/Components/ImageView.ixx",
        "source/MW/SceneGraph/Components/MeshCollider.cpp",
        "source/MW/SceneGraph/Components/MeshCollider.ixx",
        "source/MW/SceneGraph/Components/MeshRenderer.cpp",
        "source/MW/SceneGraph/Components/MeshRenderer.ixx",
        "source/MW/SceneGraph/Components/RigidBody.cpp",
        "source/MW/SceneGraph/Components/RigidBody.ixx",
        "source/MW/SceneGraph/Components/Script.ixx",
        "source/MW/SceneGraph/Components/SphereCollider.cpp",
        "source/MW/SceneGraph/Components/SphereCollider.ixx",
        "source/MW/SceneGraph/Components/TextView.cpp",
        "source/MW/SceneGraph/Components/TextView.ixx",
        "source/MW/SceneGraph/Components/View.cpp",
        "source/MW/SceneGraph/Components/View.ixx",
        "source/MW/SceneGraph/Internal/Bullet.h",
        "source/MW/SceneGraph/Internal/Bullet.ixx",
        "source/MW/SceneGraph/Internal/CapsuleShape.ixx",
        "source/MW/SceneGraph/Axis.ixx",
        "source/MW/SceneGraph/Coroutine.ixx",
        "source/MW/SceneGraph/Events.ixx",
        "source/MW/SceneGraph/LayerMask.ixx",
        "source/MW/SceneGraph/Node.cpp",
        "source/MW/SceneGraph/Node.ixx",
        "source/MW/SceneGraph/PhysicsWorld.cpp",
        "source/MW/SceneGraph/PhysicsWorld.ixx",
        "source/MW/SceneGraph/Renderable.ixx",
        "source/MW/SceneGraph/Scene.cpp",
        "source/MW/SceneGraph/Scene.ixx",
        "source/MW/SceneGraph/SceneGraph.ixx",
        "source/MW/SceneGraph/SceneRenderer.cpp",
        "source/MW/SceneGraph/SceneRenderer.ixx",
        "source/MW/System/App.cpp",
        "source/MW/System/App.ixx",
        "source/MW/System/ApplicationDispatcher.ixx",
        "source/MW/System/AsyncExecutor.ixx",
        "source/MW/System/Awaitable.ixx",
        "source/MW/System/Clock.ixx",
        "source/MW/System/Debug.h",
        "source/MW/System/Dispatcher.cpp",
        "source/MW/System/Dispatcher.ixx",
        "source/MW/System/EntryPoint.cpp",
        "source/MW/System/EventHandlerList.ixx",
        "source/MW/System/Exception.ixx",
        "source/MW/System/Executor.ixx",
        "source/MW/System/GC.ixx",
        "source/MW/System/IAwaitable.ixx",
        "source/MW/System/IAwaiter.ixx",
        "source/MW/System/Json.ixx",
        "source/MW/System/Object.cpp",
        "source/MW/System/Object.ixx",
        "source/MW/System/Path.ixx",
        "source/MW/System/Pointers.ixx",
        "source/MW/System/PostExecutor.ixx",
        "source/MW/System/Spinlock.ixx",
        "source/MW/System/SyncExecutor.ixx",
        "source/MW/System/System.ixx",
        "source/MW/System/Task.cpp",
        "source/MW/System/Task.ixx",
        "source/MW/System/ThreadPool.ixx",
        "source/MW/System/UUID.ixx",
        "source/MW/System/Window.cpp",
        "source/MW/System/Window.ixx",
        "source/MW/System/Internal/Platform.h",
        "source/MW/System/Internal/PlatformHeaders.h",
        "source/MW/Utilities/BinPacking/Bin.ixx",
        "source/MW/Utilities/BinPacking/BinPacker.cpp",
        "source/MW/Utilities/BinPacking/BinPacker.ixx",
        "source/MW/Utilities/BinPacking/BinPacking.ixx",
        "source/MW/Utilities/BinPacking/BSPNode.cpp",
        "source/MW/Utilities/BinPacking/BSPNode.ixx",
        "source/MW/Utilities/BinPacking/BSPNodeAllocator.cpp",
        "source/MW/Utilities/BinPacking/BSPNodeAllocator.ixx",
        "source/MW/Utilities/BinPacking/RectMapping.ixx",
        "source/MW/Utilities/Base64.ixx",
        "source/MW/Utilities/EnumFlags.ixx",
        "source/MW/Utilities/Sink.ixx",
        "source/MW/Utilities/Util.ixx",
        "source/MW/Utilities/Utilities.ixx",
        "source/MW/Microwave.ixx"
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
            "source/MW/Data/Database/AssetDatabase.ixx",
            "source/MW/Data/Database/AssetImporter.ixx",
            "source/MW/Data/Database/AudioClipImporter.ixx",
            "source/MW/Data/Database/Database.ixx",
            "source/MW/Data/Database/DefaultImporter.ixx",
            "source/MW/Data/Database/Metadata.ixx",
            "source/MW/Data/Database/ModelImporter.cpp",
            "source/MW/Data/Database/ModelImporter.ixx",
            "source/MW/Data/Database/TextureImporter.ixx",
            "source/MW/Data/Internal/FBX_SDK.h",
            "source/MW/Data/Internal/FBXModelConverter.cpp",
            "source/MW/Data/Internal/FBXModelConverter.ixx",
            "source/MW/Data/Internal/FBXUDPParser.ixx",
            "source/MW/Graphics/Internal/HWBufferD3D11.cpp",
            "source/MW/Graphics/Internal/HWBufferD3D11.ixx",
            "source/MW/Graphics/Internal/HWBufferOpenGL.cpp",
            "source/MW/Graphics/Internal/HWBufferOpenGL.ixx",
            "source/MW/Graphics/Internal/HWContextD3D11.cpp",
            "source/MW/Graphics/Internal/HWContextD3D11.ixx",
            "source/MW/Graphics/Internal/HWContextOpenGL.cpp",
            "source/MW/Graphics/Internal/HWContextOpenGL.ixx",
            "source/MW/Graphics/Internal/HWDriverContext.ixx",
            "source/MW/Graphics/Internal/HWDriverContextWGL.ixx",
            "source/MW/Graphics/Internal/HWRenderTextureD3D11.cpp",
            "source/MW/Graphics/Internal/HWRenderTextureD3D11.ixx",
            "source/MW/Graphics/Internal/HWRenderTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/HWRenderTextureOpenGL.ixx",
            "source/MW/Graphics/Internal/HWShaderD3D11.cpp",
            "source/MW/Graphics/Internal/HWShaderD3D11.ixx",
            "source/MW/Graphics/Internal/HWShaderOpenGL.cpp",
            "source/MW/Graphics/Internal/HWShaderOpenGL.ixx",
            "source/MW/Graphics/Internal/HWSurfaceD3D11.cpp",
            "source/MW/Graphics/Internal/HWSurfaceD3D11.ixx",
            "source/MW/Graphics/Internal/HWSurfaceWGL.cpp",
            "source/MW/Graphics/Internal/HWSurfaceWGL.ixx",
            "source/MW/Graphics/Internal/HWTextureD3D11.cpp",
            "source/MW/Graphics/Internal/HWTextureD3D11.ixx",
            "source/MW/Graphics/Internal/HWTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/HWTextureOpenGL.ixx",
            "source/MW/Graphics/Internal/OpenGLAPI.ixx",
            "source/MW/System/Internal/ApplicationDispatcherWindows.cpp",
            "source/MW/System/Internal/ApplicationDispatcherWindows.ixx",
            "source/MW/System/Internal/WindowWindows.cpp",
            "source/MW/System/Internal/WindowWindows.ixx"
        }

    filter { "system:macosx" }
        files {
            "source/MW/Graphics/Internal/HWBufferOpenGL.cpp",
            "source/MW/Graphics/Internal/HWBufferOpenGL.ixx",
            "source/MW/Graphics/Internal/HWContextOpenGL.cpp",
            "source/MW/Graphics/Internal/HWContextOpenGL.ixx",
            "source/MW/Graphics/Internal/HWDriverContext.ixx",
            "source/MW/Graphics/Internal/HWDriverContextNSGL.ixx",
            "source/MW/Graphics/Internal/HWRenderTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/HWRenderTextureOpenGL.ixx",
            "source/MW/Graphics/Internal/HWShaderOpenGL.cpp",
            "source/MW/Graphics/Internal/HWShaderOpenGL.ixx",
            "source/MW/Graphics/Internal/HWSurfaceNSGL.cpp",
            "source/MW/Graphics/Internal/HWSurfaceNSGL.ixx",
            "source/MW/Graphics/Internal/HWTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/HWTextureOpenGL.ixx",
            "source/MW/Graphics/Internal/OpenGLAPI.ixx",
            "source/MW/System/Internal/ApplicationDispatcherMacOS.cpp",
            "source/MW/System/Internal/ApplicationDispatcherMacOS.ixx",
            "source/MW/System/Internal/WindowMacOS.cpp",
            "source/MW/System/Internal/WindowMacOS.ixx"
        }

    filter { "system:ios" }
        files {
            "source/MW/Graphics/Internal/HWBufferOpenGL.cpp",
            "source/MW/Graphics/Internal/HWBufferOpenGL.ixx",
            "source/MW/Graphics/Internal/HWContextOpenGL.cpp",
            "source/MW/Graphics/Internal/HWContextOpenGL.ixx",
            "source/MW/Graphics/Internal/HWDriverContext.ixx",
            "source/MW/Graphics/Internal/HWDriverContextEAGL.ixx",
            "source/MW/Graphics/Internal/HWRenderTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/HWRenderTextureOpenGL.ixx",
            "source/MW/Graphics/Internal/HWShaderOpenGL.cpp",
            "source/MW/Graphics/Internal/HWShaderOpenGL.ixx",
            "source/MW/Graphics/Internal/HWSurfaceEAGL.cpp",
            "source/MW/Graphics/Internal/HWSurfaceEAGL.ixx",
            "source/MW/Graphics/Internal/HWTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/HWTextureOpenGL.ixx",
            "source/MW/Graphics/Internal/OpenGLAPI.ixx",
            "source/MW/System/Internal/ApplicationDispatcherIOS.cpp",
            "source/MW/System/Internal/ApplicationDispatcherIOS.ixx",
            "source/MW/System/Internal/WindowIOS.cpp",
            "source/MW/System/Internal/WindowIOS.ixx"
        }

    filter { "system:android" }
        sysincludedirs {
            "$(NDK_ROOT)/sources/android/native_app_glue"
        }
        files {
            "source/MW/Graphics/Internal/HWBufferOpenGL.cpp",
            "source/MW/Graphics/Internal/HWBufferOpenGL.ixx",
            "source/MW/Graphics/Internal/HWContextOpenGL.cpp",
            "source/MW/Graphics/Internal/HWContextOpenGL.ixx",
            "source/MW/Graphics/Internal/HWDriverContext.ixx",
            "source/MW/Graphics/Internal/HWDriverContextEAGL.ixx",
            "source/MW/Graphics/Internal/HWRenderTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/HWRenderTextureOpenGL.ixx",
            "source/MW/Graphics/Internal/HWShaderOpenGL.cpp",
            "source/MW/Graphics/Internal/HWShaderOpenGL.ixx",
            "source/MW/Graphics/Internal/HWSurfaceEAGL.cpp",
            "source/MW/Graphics/Internal/HWSurfaceEAGL.ixx",
            "source/MW/Graphics/Internal/HWTextureOpenGL.cpp",
            "source/MW/Graphics/Internal/HWTextureOpenGL.ixx",
            "source/MW/Graphics/Internal/OpenGLAPI.ixx",
            "source/MW/IO/AndroidAssetStream.h",
            "source/MW/System/Internal/ApplicationDispatcherAndroid.cpp",
            "source/MW/System/Internal/ApplicationDispatcherAndroid.ixx",
            "source/MW/System/Internal/WindowAndroid.cpp",
            "source/MW/System/Internal/WindowAndroid.ixx"
        }

    filter { "action:vs*" }
        objdir ("!obj/$(PlatformName)/$(Configuration)")
        targetdir ("lib/$(PlatformName)/$(Configuration)")
        moduleoutputfile ("bmi/$(PlatformName)/$(Configuration)")
        characterset "MBCS"

    filter { "action:xcode*" }
        objdir ("!obj/$(PLATFORM_NAME)/$(CONFIGURATION)")
        targetdir ("lib/$(PLATFORM_NAME)/$(CONFIGURATION)")

    filter { "action:android*" }
        objdir ("obj/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)")
        targetdir ("lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)")

    filter "configurations:Debug"
        defines {
            "NOMINMAX",
            "AL_ALEXT_PROTOTYPES",
            "WIN32",
            "_DEBUG"
        }
        symbols "On"

    filter "configurations:Release"
        defines {
            "NOMINMAX",
            "AL_ALEXT_PROTOTYPES",
            "WIN32",
            "NDEBUG"
        }
        optimize "Speed"
        inlining "Auto"
        floatingpoint "Fast"
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
        cppdialect "C++latest"
        cdialect "C11"
        toolset "v143"
        scanformoduledependencies "true"
        enablemodules "true"
        disablewarnings {
            "4005", "5106", -- Windows.h "macro already defined..."
            "4251" -- FBX SDK "needs to have dll-interface to be used..."
        }
        buildoptions {
            "/await",
        }
        