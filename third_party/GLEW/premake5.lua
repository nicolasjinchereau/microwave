require "android-lib"

workspace "glew"
    configurations { "Debug", "Release" }
    location ("projects/" .. os.target())

project "glew"
    kind "StaticLib"
    language "C"
    
    location ("projects/" .. os.target())
    targetname "glew"

    files {
        "include/glew.h",
        "include/wglew.h",
        "src/glew.c"
    }

    sysincludedirs {
        "include",
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
        defines {
            "DEBUG",
            "_DEBUG",
            "WIN32",
            "WIN32_LEAN_AND_MEAN",
            "VC_EXTRALEAN",
            "GLEW_STATIC",
            "_LIB",
        }
        symbols "On"

    filter "configurations:Release"
        defines {
            "NDEBUG",
            "WIN32",
            "WIN32_LEAN_AND_MEAN",
            "VC_EXTRALEAN",
            "GLEW_STATIC",
            "_LIB",
        }
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
        