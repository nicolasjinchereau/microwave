/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once

#if _WIN32
  #if defined(_M_ARM)
    #define PLATFORM_WINDOWS_ARM 1
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

#ifndef PLATFORM_WINDOWS
  #define PLATFORM_WINDOWS 0
#endif

#ifndef PLATFORM_WINDOWS_ARM
  #define PLATFORM_WINDOWS_ARM 0
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
