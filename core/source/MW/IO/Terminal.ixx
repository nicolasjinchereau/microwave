/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.IO.Terminal;
import Microwave.System.Exception;
import <MW/System/Debug.h>;
import <iostream>;
import <syncstream>;
import <type_traits>;
import <utility>;

template<class T>
auto check_null(T&& value) {
    if constexpr (std::is_pointer_v<std::decay_t<T>>) {
        Assert(value);
    }
    return std::forward<T>(value);
}

export namespace mw {
inline namespace system {

template<class... Args>
void writeln(Args&&... args)
{
    std::osyncstream sout(std::cout);
    ((sout << check_null(std::forward<Args>(args))), ...) << '\n';
}

} // system
} // mw
