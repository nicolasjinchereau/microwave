require "android-lib"

workspace "hlslparser"
    configurations { "Debug", "Release" }
    location ("projects/" .. os.target())

project "hlslparser"
    kind "StaticLib"
    language "C"
    --architecture "x86_64"

    location ("projects/" .. os.target())
    targetname "hlslparser"

    files {
        "src/CodeWriter.h",
        "src/Engine.h",
        "src/GLSLGenerator.h",
        "src/HLSLGenerator.h",
        "src/HLSLParser.h",
        "src/HLSLTokenizer.h",
        "src/HLSLTree.h",
        "src/MSLGenerator.h",
        "src/CodeWriter.cpp",
        "src/Engine.cpp",
        "src/GLSLGenerator.cpp",
        "src/HLSLGenerator.cpp",
        "src/HLSLParser.cpp",
        "src/HLSLTokenizer.cpp",
        "src/HLSLTree.cpp",
        "src/MSLGenerator.cpp"
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