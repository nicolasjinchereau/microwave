/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/Console.h>
#include <MW/System/Platforms.h>

#if defined(_MSC_VER)
#include <Windows.h>
#endif

#if PLATFORM_ANDROID
#  include <android/log.h>
#else
#  include <iostream>
#endif

namespace mw {
inline namespace system {

std::mutex Console::mut;

void Console::WriteToConsole(const char* text)
{
#if PLATFORM_ANDROID
    __android_log_write(ANDROID_LOG_INFO, "Microwave", text);
#else
#  if defined(_MSC_VER) // write to visual studio Output window
    std::string buffer;
    buffer.reserve(strlen(text) + 1);
    buffer += text;
    buffer += '\n';
    OutputDebugStringA(buffer.c_str());
#  else
    std::cout << text << std::endl;
#  endif
#endif
}

} // system
} // mw
