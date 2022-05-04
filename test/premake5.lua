require "android-app"

workspace "Test"
    configurations { "Debug", "Release" }
    location ("projects/" .. os.target())

project "Test"
    kind "ConsoleApp"
    language "C++"
    --architecture "x86_64"

    location ("projects/" .. os.target())
    targetname "Test"

    sysincludedirs {
        "../core/source"
    }

    files {
        "source/BatteryMeter.ixx",
        "source/BigDoors.ixx",
        "source/CameraController.ixx",
        "source/Coin.cpp",
        "source/Coin.ixx",
        "source/CoinCounter.ixx",
        "source/ExitDoorTrigger.cpp",
        "source/ExitDoorTrigger.ixx",
        "source/Game.ixx",
        "source/LossScreen.cpp",
        "source/LossScreen.ixx",
        "source/Player.ixx",
        "source/PlayerWheel.cpp",
        "source/PlayerWheel.ixx",
        "source/SpinningGear.ixx",
        "source/TestApplication.cpp",
        "source/WinScreen.cpp",
        "source/WinScreen.ixx"
    }

    xcodebuildresources {
        "assets/data/manifest.json",
        "fdc48bb8898e496abf32281bb6fcdf77",
        "6abf328bb8898e49fcdf77281bb6fdc4"
    }

    androidabis {
		"armeabi-v7a", "arm64-v8a"
    }

    androidminsdkversion "android-16"

    filter { "action:vs*" }
        objdir ("!obj/$(PlatformName)/$(Configuration)")
        targetdir ("bin/$(PlatformName)/$(Configuration)")
        debugdir "assets"
        characterset "MBCS"
        libdirs {
            "../core/lib/$(PlatformName)/$(Configuration)",
            "../third_party/bullet/lib/$(PlatformName)/$(Configuration)",
            "../third_party/dr-mp3/lib/$(PlatformName)/$(Configuration)",
            "../third_party/fbx/lib/vs2019/$(PlatformName)/$(Configuration)",
            "../third_party/freetype2/lib/$(PlatformName)/$(Configuration)",
            "../third_party/GLEW/lib/$(PlatformName)/$(Configuration)",
            "../third_party/hlslparser/lib/$(PlatformName)/$(Configuration)",
            "../third_party/libjpeg/lib/$(PlatformName)/$(Configuration)",
            "../third_party/libpng/lib/$(PlatformName)/$(Configuration)",
            "../third_party/libzip/lib/$(PlatformName)/$(Configuration)",
            "../third_party/ogg/lib/$(PlatformName)/$(Configuration)",
            "../third_party/openal-soft/lib/$(PlatformName)/$(Configuration)",
            "../third_party/tinyexr/lib/$(PlatformName)/$(Configuration)",
            "../third_party/tinyxml2/lib/$(PlatformName)/$(Configuration)",
            "../third_party/vorbis/lib/$(PlatformName)/$(Configuration)",
            "../third_party/xz-utils/lib/$(PlatformName)/$(Configuration)",
            "../third_party/zlib/lib/$(PlatformName)/$(Configuration)"
        }
        
    filter { "action:xcode*" }
        objdir ("!obj/$(PLATFORM_NAME)/$(CONFIGURATION)")
        targetdir ("bin/$(PLATFORM_NAME)/$(CONFIGURATION)")
        libdirs {
            "../core/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/bullet/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/dr-mp3/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/freetype2/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/GLEW/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/hlslparser/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/libjpeg/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/libpng/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/libzip/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/ogg/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/openal-soft/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/tinyexr/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/tinyxml2/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/vorbis/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/xz-utils/lib/$(PLATFORM_NAME)/$(CONFIGURATION)",
            "../third_party/zlib/lib/$(PLATFORM_NAME)/$(CONFIGURATION)"
        }

    filter { "action:android*" }
        objdir ("obj/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)")
        targetdir ("bin/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)")
        libdirs {
            "../core/lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/bullet/lib/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/dr-mp3/lib/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/freetype2/lib/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/GLEW/lib/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/hlslparser/lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/libjpeg/lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/libpng/lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/libzip/lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/ogg/lib/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/openal-soft/lib/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/tinyexr/lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/tinyxml2/lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/vorbis/lib/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/xz-utils/lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)",
            "../third_party/zlib/lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)"
        }

    filter "configurations:Debug"
        defines {
            "NOMINMAX",
            "DEBUG",
            "_DEBUG"
        }
        symbols "On"

    filter "configurations:Release"
        defines {
            "NOMINMAX",
            "NDEBUG"
        }
        optimize "Speed"
        inlining "Auto"
        floatingpoint "Fast"
        vectorextensions "SSE2"

    filter { "system:ios" }
        links {
            "QuartzCore.framework",
            "CoreMedia.framework",
            "CoreVideo.framework",
            "CoreFoundation.framework",
            "GLKit.framework",
            "UIKit.framework",
            "OpenGLES.framework",
            "CoreGraphics.framework",
            "Foundation.framework",
            "CoreImage.framework",
            "microwave",
            "freetype2",
            "glew",
            "hlslparser",
            "jpeg",
            "png",
            "zip",
            "ogg",
            "openal-soft",
            "tinyexr",
            "tinyxml2",
            "vorbis",
            "xz-utils",
            "zlib"
        }
        files {
            "plists/ios/Info.plist"
        }
        xcodebuildsettings {
            ["INFOPLIST_FILE"] = "../../plists/ios/Info.plist";
        }

    filter { "system:macosx" }
        links {
            "Foundation.framework",
            "OpenGL.framework",
            "Carbon.framework",
            "AppKit.framework",
            "microwave",
            "freetype2",
            "glew",
            "hlslparser",
            "jpeg",
            "png",
            "zip",
            "ogg",
            "openal-soft",
            "tinyexr",
            "tinyxml2",
            "vorbis",
            "xz-utils",
            "zlib"
        }
        files {
            "plists/macosx/Info.plist"
        }
        xcodebuildsettings {
            ["INFOPLIST_FILE"] = "../../plists/macosx/Info.plist";
        }

    filter { "system:windows" }
        links {
            "microwave", -- omit extension so premake uses externalproject below
            "BulletCollision.lib",
            "BulletDynamics.lib",
            "BulletSoftBody.lib",
            "LinearMath.lib",
            "dr-mp3.lib",
            "libfbxsdk.lib",
            "freetype2.lib",
            "glew.lib",
            "hlslparser.lib",
            "jpeg.lib",
            "png.lib",
            "zip.lib",
            "ogg.lib",
            "openal-soft.lib",
            "tinyexr.lib",
            "tinyxml2.lib",
            "vorbis.lib",
            "xz-utils.lib",
            "zlib.lib",
            "d3dcompiler.lib",
            "D3D11.lib",
            "opengl32.lib",
            "OneCore.lib",
            "winmm.lib"
        }
        postbuildcommands {
            "xcopy /y /d  \"$(ProjectDir)..\\..\\..\\third_party\\fbx\\lib\\vs2019\\$(PlatformName)\\$(Configuration)\\libfbxsdk.dll\" \"$(TargetDir)\""
        }

    filter { "system:android" }
        files {
            -- add AndroidManifest.xml...
        }
        links {
            -- add android system libraries..
            "microwave",
            "freetype2",
            "glew",
            "hlslparser",
            "jpeg",
            "png",
            "zip",
            "ogg",
            "openal-soft",
            "tinyexr",
            "tinyxml2",
            "vorbis",
            "xz-utils",
            "zlib"
        }

    local baseSDK = ""

    if _ACTION == "xcode4" then
        if os.target() == "ios" then
            baseSDK = "iphoneos"
        elseif os.target() == "macosx" then
            baseSDK = "macosx"
        end
    end

    filter { "action:xcode*" }
        buildoptions { "-fcoroutines-ts" }
        xcodebuildsettings {
            ["PRODUCT_BUNDLE_IDENTIFIER"] = "com.company.Test";
            ["DEVELOPMENT_TEAM"] = "";
            ["ARCHS"] = "$(ARCHS_STANDARD)";
            ["SDKROOT"] = baseSDK;
            ["GCC_INPUT_FILETYPE"] = "sourcecode.cpp.objcpp"; -- compile *.cpp as Objective-C++
            ["CLANG_ENABLE_OBJC_ARC"] = "YES";
            ["CLANG_CXX_LANGUAGE_STANDARD"] = "gnu++20";
            ["GCC_C_LANGUAGE_STANDARD"] = "gnu11";
            ["CLANG_CXX_LIBRARY"] = "libc++";
            ["COMPRESS_PNG_FILES"] = "NO";
            ["ASSETCATALOG_COMPILER_OPTIMIZATION"] = "";
        }
    
    filter { "action:vs*" }
        cppdialect "C++latest"
        cdialect "C11"
        toolset "v143"
        scanformoduledependencies "true"
        enablemodules "true"
        moduledependencies {
            "../core/bmi/$(PlatformName)/$(Configuration)"
        }
        buildoptions {
            "/await"
        }
        externalproject "microwave"
            location "../core/projects/windows"
            uuid "F21BE648-5E86-9ABF-A7C4-4B65136E7814"
            kind "StaticLib"
            language "C++"
            --architecture "x86_64"