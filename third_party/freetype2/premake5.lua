require "android-lib"

workspace "freetype2"
    configurations { "Debug", "Release" }
    location ("projects/" .. os.target())

project "freetype2"
    kind "StaticLib"
    language "C"
    
    location ("projects/" .. os.target())
    targetname "freetype2"

    files {
        "source/autofit/autofit.c",
        "source/base/ftbase.c",
        "source/base/ftbbox.c",
        "source/base/ftbdf.c",
        "source/base/ftbitmap.c",
        "source/base/ftcid.c",
        "source/base/ftdebug.c",
        "source/base/ftfstype.c",
        "source/base/ftgasp.c",
        "source/base/ftglyph.c",
        "source/base/ftgxval.c",
        "source/base/ftinit.c",
        "source/base/ftmm.c",
        "source/base/ftotval.c",
        "source/base/ftpatent.c",
        "source/base/ftpfr.c",
        "source/base/ftstroke.c",
        "source/base/ftsynth.c",
        "source/base/ftsystem.c",
        "source/base/fttype1.c",
        "source/base/ftwinfnt.c",
        "source/bdf/bdf.c",
        "source/bzip2/ftbzip2.c",
        "source/cache/ftcache.c",
        "source/cff/cff.c",
        "source/cid/type1cid.c",
        "source/gzip/ftgzip.c",
        "source/lzw/ftlzw.c",
        "source/pcf/pcf.c",
        "source/pfr/pfr.c",
        "source/psaux/psaux.c",
        "source/pshinter/pshinter.c",
        "source/psnames/psnames.c",
        "source/raster/raster.c",
        "source/sfnt/sfnt.c",
        "source/smooth/smooth.c",
        "source/truetype/truetype.c",
        "source/type1/type1.c",
        "source/type42/type42.c",
        "source/winfonts/winfnt.c"
    }

    sysincludedirs {
        "include/",
        "../zlib/source/"
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
            "_LIB",
            "FT2_BUILD_LIBRARY",
            "FT_DEBUG_LEVEL_ERROR",
            "FT_DEBUG_LEVEL_TRACE",
            "_CRT_SECURE_NO_DEPRECATE",
            "_CRT_SECURE_NO_WARNINGS"
        }
        symbols "On"

    filter "configurations:Release"
        defines {
            "NDEBUG",
            "WIN32",
            "_LIB",
            "FT2_BUILD_LIBRARY",
            "_CRT_SECURE_NO_DEPRECATE",
            "_CRT_SECURE_NO_WARNINGS"
        }
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
        