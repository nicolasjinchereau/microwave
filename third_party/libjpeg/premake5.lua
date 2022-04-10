require "android-lib"

workspace "libjpeg"
    configurations { "Debug", "Release" }
    location ("projects/" .. os.target())

project "libjpeg"
    kind "StaticLib"
    language "C"
    
    location ("projects/" .. os.target())
    targetname "jpeg"

    files {
        "source/jaricom.c",
        "source/jcapimin.c",
        "source/jcapistd.c",
        "source/jcarith.c",
        "source/jccoefct.c",
        "source/jccolor.c",
        "source/jcdctmgr.c",
        "source/jchuff.c",
        "source/jcinit.c",
        "source/jcmainct.c",
        "source/jcmarker.c",
        "source/jcmaster.c",
        "source/jcomapi.c",
        "source/jcparam.c",
        "source/jcprepct.c",
        "source/jcsample.c",
        "source/jctrans.c",
        "source/jdapimin.c",
        "source/jdapistd.c",
        "source/jdarith.c",
        "source/jdatadst.c",
        "source/jdatasrc.c",
        "source/jdcoefct.c",
        "source/jdcolor.c",
        "source/jddctmgr.c",
        "source/jdhuff.c",
        "source/jdinput.c",
        "source/jdmainct.c",
        "source/jdmarker.c",
        "source/jdmaster.c",
        "source/jdmerge.c",
        "source/jdpostct.c",
        "source/jdsample.c",
        "source/jdtrans.c",
        "source/jerror.c",
        "source/jfdctflt.c",
        "source/jfdctfst.c",
        "source/jfdctint.c",
        "source/jidctflt.c",
        "source/jidctfst.c",
        "source/jidctint.c",
        "source/jmemmgr.c",
        "source/jmemnobs.c",
        "source/jquant1.c",
        "source/jquant2.c",
        "source/jutils.c"
    }

    sysincludedirs {
        "source/",
        "/../zlib/source/"
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