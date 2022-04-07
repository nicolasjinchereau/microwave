/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Json.h>
#include <unordered_map>

#if !defined(_MSC_VER)
#  include <signal.h>
#endif

// PLATFORM TYPE
#if _WIN32
  #if defined(_M_ARM)
    #define PLATFORM_WINDOWS_PHONE 1
  #else
    #define PLATFORM_WINDOWS 1
  #endif
#elif __APPLE__
  #include "TargetConditionals.h"
  #if TARGET_IPHONE_SIMULATOR
    #define PLATFORM_IOS 1
  #elif TARGET_OS_IPHONE
    #define PLATFORM_IOS 1
  #elif TARGET_OS_MAC
    #define PLATFORM_MACOS 1
  #endif
#elif __ANDROID__
  #define PLATFORM_ANDROID 1
#endif

// GRAPHICS DRIVER
#if _WIN32
  #if defined(_M_ARM)
    #define GFX_DRIVER_D3D 1
  #else
    //#define GFX_DRIVER_D3D 1
    #define GFX_DRIVER_OPENGL 1
  #endif
#else
  #define GFX_DRIVER_OPENGL 1
#endif

// define 0 for undefined preprocessors
#ifndef PLATFORM_WINDOWS
  #define PLATFORM_WINDOWS 0
#endif

#ifndef PLATFORM_WINDOWS_PHONE
  #define PLATFORM_WINDOWS_PHONE 0
#endif

#ifndef PLATFORM_MACOS
  #define PLATFORM_MACOS 0
#endif

#ifndef PLATFORM_IOS
  #define PLATFORM_IOS 0
#endif

#ifndef PLATFORM_ANDROID
  #define PLATFORM_ANDROID 0
#endif

#ifndef PLATFORM_D3D
  #define PLATFORM_D3D 0
#endif

#ifndef PLATFORM_OPENGL
  #define PLATFORM_OPENGL 0
#endif

namespace mw {

enum class Platform : int
{
    Windows,
    MacOS,
    iOS,
    Android
};

inline Platform GetCurrentPlatform()
{
#ifdef PLATFORM_WINDOWS
    return Platform::Windows;
#elif PLATFORM_MACOS
    return Platform::MacOS;
#elif PLATFORM_IOS
    return Platform::iOS;
#elif PLATFORM_ANDROID
    return Platform::Android;
#else
#  error unsupported platform
#endif
}

inline const char* GetPlatformName(Platform platform)
{
    static std::unordered_map<Platform, std::string> platformNames {
        { Platform::Windows, "windows" },
        { Platform::MacOS, "macos" },
        { Platform::iOS, "ios" },
        { Platform::Android, "android" }
    };

    return platformNames[platform].c_str();
}

inline Platform GetNamedPlatform(const std::string& platform)
{
    static std::unordered_map<std::string, Platform> platformsByName {
        { "windows", Platform::Windows },
        { "macos", Platform::MacOS },
        { "ios", Platform::iOS },
        { "android", Platform::Android }
    };

    return platformsByName[platform];
}

inline void to_json(json& obj, const Platform& platform) {
    obj = GetPlatformName(platform);
}

inline void from_json(const json& obj, Platform& platform) {
    platform = GetNamedPlatform(obj.get<std::string>());
}

#ifdef _MSC_VER
#  define DEBUG_BREAK() __debugbreak()
#else
#  error not tested
#  define DEBUG_BREAK() raise(SIGTRAP)
#endif

}
