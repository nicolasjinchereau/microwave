/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <mutex>
#include <string>
#include <iomanip>
#include <MW/Utilities/Format.h>

namespace mw {
inline namespace system {

class Console
{
    static std::mutex mut;
    static void WriteToConsole(const char* text);
    
public:
    template<typename... Args>
    static void WriteLine(const char* fmt, Args&& ... args)
    {
        std::lock_guard<std::mutex> lk(mut);
        auto str = format(fmt, std::forward<Args>(args)...);
        WriteToConsole(str.c_str());
    }
};

} // system
} // mw
