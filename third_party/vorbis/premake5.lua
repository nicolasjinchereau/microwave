require "android-lib"

workspace "vorbis"
    configurations { "Debug", "Release" }
    location ("projects/" .. os.target())

project "vorbis"
    kind "StaticLib"
    language "C"
    
    location ("projects/" .. os.target())
    targetname "vorbis"

    files {
        "src/backends.h",
        "src/bitrate.h",
        "src/codebook.h",
        "src/codec_internal.h",
        "src/envelope.h",
        "src/highlevel.h",
        "src/lookup.h",
        "src/lookup_data.h",
        "src/lpc.h",
        "src/lsp.h",
        "src/masking.h",
        "src/mdct.h",
        "src/misc.h",
        "src/os.h",
        "src/psy.h",
        "src/registry.h",
        "src/scales.h",
        "src/smallft.h",
        "src/window.h",
        "src/analysis.c",
        "src/bitrate.c",
        "src/block.c",
        "src/codebook.c",
        "src/envelope.c",
        "src/floor0.c",
        "src/floor1.c",
        "src/info.c",
        "src/lookup.c",
        "src/lpc.c",
        "src/lsp.c",
        "src/mapping0.c",
        "src/mdct.c",
        "src/psy.c",
        "src/registry.c",
        "src/res0.c",
        "src/sharedbook.c",
        "src/smallft.c",
        "src/synthesis.c",
        "src/vorbisenc.c",
        "src/vorbisfile.c",
        "src/window.c"
    }

    sysincludedirs {
        "include",
        "../ogg/include"
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

    filter {}
        defines {
            "WIN32",
            "_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS",
            "_CRT_SECURE_NO_DEPRECATE",
            "_CRT_NONSTDC_NO_DEPRECATE"
        }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Speed"
        inlining "Auto"
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