require "android-lib"

workspace "tinyexr"
    configurations { "Debug", "Release" }
    location ("projects/" .. os.target())

project "tinyexr"
    kind "StaticLib"
    language "C"
    
    location ("projects/" .. os.target())
    targetname "tinyexr"

    files {
        "src/tinyexr.cpp",
        "zfp/src/bitstream.c",
        "zfp/src/decode1d.c",
        "zfp/src/decode1f.c",
        "zfp/src/decode2d.c",
        "zfp/src/decode2f.c",
        "zfp/src/decode3d.c",
        "zfp/src/decode3f.c",
        "zfp/src/encode1d.c",
        "zfp/src/encode1f.c",
        "zfp/src/encode2d.c",
        "zfp/src/encode2f.c",
        "zfp/src/encode3d.c",
        "zfp/src/encode3f.c",
        "zfp/src/zfp.c"
    }

    sysincludedirs {
        "include",
        "zfp/inc"
    }

    androidabis {
		"armeabi-v7a", "arm64-v8a"
    }

    androidminsdkversion "android-16"
    
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
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Speed"
        inlining "Auto"
        floatingpoint "Fast"
        vectorextensions "SSE2"

    filter { "action:xcode*" }
        xcodebuildsettings {           
            ["CLANG_CXX_LANGUAGE_STANDARD"] = "gnu++20";
            ["GCC_C_LANGUAGE_STANDARD"] = "gnu11";
            ["CLANG_CXX_LIBRARY"] = "libc++";
        }
    
    filter { "action:vs*" }
        cppdialect "C++20"
        cdialect "C11"