/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Console;
import Microwave.Utilities.Format;
import Microwave.System.Spinlock;
import <mutex>;
import <string>;
import <iomanip>;
import <utility>;

export namespace mw {
inline namespace system {

class Console
{
    //inline static std::mutex mut;
    inline static Spinlock mut;
    static void WriteToConsole(const char* text);
    
public:
    template<typename... Args>
    static void WriteLine(const char* fmt, Args&& ... args)
    {
        std::lock_guard<Spinlock> lk(mut);
        auto str = format(fmt, std::forward<Args>(args)...);
        WriteToConsole(str.c_str());
    }
};

} // system
} // mw
