/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#include <mw/system/internal/platform.h>

module Microwave.System.Console;

#if PLATFORM_ANDROID
  import <android/log.h>;
#else
  import <iostream>;
#endif

namespace mw {
inline namespace system {

void Console::WriteToConsole(const char* text)
{
#if PLATFORM_ANDROID
    __android_log_write(ANDROID_LOG_INFO, "Microwave", text);
#else
    std::cout << text << std::endl;
#endif
}

} // system
} // mw
