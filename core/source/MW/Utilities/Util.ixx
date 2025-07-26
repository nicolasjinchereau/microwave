/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Utilities.Util;
import Microwave.System.Path;
import std;

export namespace mw {
inline namespace utilities {

void ToLower(std::string& str)
{
    for (auto& c : str)
        c = (char)std::tolower(c);
}

std::string ToLower(const std::string& str)
{
    std::string ret = str;
    ToLower(ret);
    return ret;
}

void ToUpper(std::string& str)
{
    for (auto& c : str)
        c = (char)std::toupper(c);
}

std::string ToUpper(const std::string& str)
{
    std::string ret = str;
    ToUpper(ret);
    return ret;
}

std::string GetExtension(const path& p)
{
    return ToLower(p.extension().string());
}

//std::size_t ReplaceAll(std::string& inout, std::string_view what, std::string_view with)
//{
//    std::size_t count = 0;
//
//    for (std::string::size_type pos = 0;
//        (pos = inout.find(what.data(), pos, what.length())) != std::string::npos;
//        pos += with.length(), ++count)
//    {
//        inout.replace(pos, what.length(), with.data(), with.length());
//    }
//
//    return count;
//}

std::string ReplaceAll(const std::string& in, std::string_view what, std::string_view with)
{
    std::string result;
    result.reserve(in.length());

    std::string::size_type first = 0;

    for (std::string::size_type last = 0;
        (last = in.find(what.data(), first, what.length())) != std::string::npos;
        first = last + what.length())
    {
        result.append(in.data() + first, in.data() + last);
        result.append(with);
    }

    if (first != in.length())
        result.append(in.data() + first, in.data() + in.length());

    return result;
}

template<class T, class S, class Compare = std::less<T>>
auto InsertSorted(std::vector<T>& cont, S&& val, Compare comp)
{
    auto it = std::upper_bound(cont.begin(), cont.end(), val, comp);
    return cont.insert(it, std::forward<S>(val));
}

template<class T>
std::span<std::byte> MakeByteSpan(T& obj) {
    return std::span<std::byte>((std::byte*)&obj, sizeof(T));
}

template<class T>
struct EnumHash
{
    typedef typename std::underlying_type<T>::type underlying_type;

    typename std::hash<underlying_type>::result_type operator()(const T& arg) const
    {
        std::hash<underlying_type> hasher;
        return hasher(static_cast<underlying_type>(arg));
    }
};

} // utilities
} // mw
