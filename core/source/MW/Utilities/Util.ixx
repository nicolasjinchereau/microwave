/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Utilities.Util;
import Microwave.System.Path;
import <algorithm>;
import <cstddef>;
import <span>;
import <string>;
import <vector>;
import <type_traits>;
import <functional>;

export namespace mw {
inline namespace utilities {

void ToLower(std::string& str)
{
    for (auto& c : str)
        c = (char)tolower(c);
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
        c = (char)toupper(c);
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

template<class T, class S, class Compare = std::less<T>>
static auto InsertSorted(std::vector<T>& cont, S&& val, Compare comp)
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
