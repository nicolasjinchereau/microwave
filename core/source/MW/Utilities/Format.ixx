/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Utilities.Format;
import <string>;
import <sstream>;
import <iostream>;
import <cassert>;
import <cstring>;
import <type_traits>;
import <mutex>;
import <stdexcept>;

namespace detail
{
    inline const char* write_until_placeholder(std::ostream& stream, const char* str)
    {
        assert(str);

        auto start = str;
        auto ph = strchr(start, '%');

        while((ph = strchr(start, '%')) && *(ph + 1) == '%')
        {
            stream.write(start, ph - start + 1);
            start = ph + 2;
            ph = strchr(start, '%');
        }

        stream.write(start, ph ? (ph - start) : strlen(start));
        return ph;
    }

    inline void format_impl(std::ostream& stream, const char* fmt)
    {
        auto ph = write_until_placeholder(stream, fmt);
        assert(!ph); // too many placeholders
    }

    template<typename Arg>
    inline void format_impl(std::ostream& stream, const char* fmt, Arg&& arg)
    {
        auto ph = write_until_placeholder(stream, fmt);
        assert(ph); // not enough placeholders

        stream << arg;
        fmt = ph + 1;

        if(*fmt != 0) {
            auto ph = write_until_placeholder(stream, fmt);
            assert(!ph); // too many placeholders
        }
    }

    template<typename Arg, typename... Args>
    inline void format_impl(std::ostream& stream, const char* fmt, Arg&& arg, Args&&... args)
    {
        auto ph = write_until_placeholder(stream, fmt);
        assert(ph); // not enough placeholders
        
        stream << arg;
        fmt = ph + 1;

        if(*fmt != 0)
            format_impl(stream, fmt, std::forward<Args>(args)...);
    }
}

export namespace mw {
inline namespace utilities {

///<summary>
///Returns a copy of 'fmt' where all '%' placeholders have been
///replaced by 'args'. The number of '%'s must match the number
///of 'args'. A literal '%' character can be written using '%%'.
///</summary>
template<typename... Args>
std::string format(const char* fmt, Args&&... args) {
    std::stringstream ss;
    detail::format_impl(ss, fmt, std::forward<Args>(args)...);
    return ss.str();
}

///<summary>
///Writes a copy of 'fmt' to 'stream' where all '%' placeholders
///have been replaced by 'args'. The number of '%'s must match the
///number of 'args'. A literal '%' character can be written using '%%'.
///</summary>
template<typename... Args>
void format(std::ostream& stream, const char* fmt, Args&&... args) {
    detail::format_impl(stream, fmt, std::forward<Args>(args)...);
}

///<summary>
///Writes a copy of 'fmt' to 'stream' where all '%' placeholders
///have been replaced by 'args'. The number of '%'s must match the
///number of 'args'. A literal '%' character can be written using '%%'.
///The 'lock' is locked before writing to, and flushing 'stream', and
///then unlocked before the function returns.
///</summary>
template<typename Lock, typename... Args>
void format(Lock& lock, std::ostream& stream, const char* fmt, Args&&... args)
{
    std::lock_guard<Lock> lk(lock);
    detail::format_impl(stream, fmt, std::forward<Args>(args)...);
    stream.flush();
}

///<summary>
///Calls format(...) passing std::cout as the 'stream' argument.
///</summary>
template<typename... Args>
void write(const char* fmt, Args&&... args) {
    detail::format_impl(std::cout, fmt, std::forward<Args>(args)...);
}

///<summary>
///Calls format(...) passing std::cout as the 'stream' argument.
///The 'lock' is locked before calling format, and flushing
///std::cout, and then unlocked before the function returns.
///</summary>
template<typename Lock, typename... Args>
void write(Lock& lock, const char* fmt, Args&&... args) {
    std::lock_guard<Lock> lk(lock);
    detail::format_impl(std::cout, fmt, std::forward<Args>(args)...);
    std::cout.flush();
}

///<summary>
///Calls format(...) passing std::cout as the 'stream' argument,
///then writes std::endl to the stream before returning.
///</summary>
template<typename... Args>
void writeln(const char* fmt, Args&&... args) {
    detail::format_impl(std::cout, fmt, std::forward<Args>(args)...);
    std::cout << std::endl;
}

///<summary>
///Calls format(...) passing std::cout as the 'stream' argument,
///then writes std::endl to the stream. The 'lock' is locked before
///calling format, and flushing std::cout, and then unlocked before
///the function returns.
///</summary>
template<typename Lock, typename... Args>
void writeln(Lock& lock, const char* fmt, Args&&... args) {
    std::lock_guard<Lock> lk(lock);
    detail::format_impl(std::cout, fmt, std::forward<Args>(args)...);
    std::cout << std::endl;
    std::cout.flush();
}

class FormattedError : public std::runtime_error
{
public:
    template<typename... Args>
    FormattedError(const char* fmt, Args&&... args)
        : std::runtime_error(format(fmt, std::forward<Args>(args)...))
    {
    }
};

} // utilities
} // mw
