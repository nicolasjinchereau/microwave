/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <string>
#include <algorithm>
#include <vector>
#include <span>
#include <MW/System/path.h>

namespace mw {
inline namespace utilities {

inline void ToLower(std::string& str)
{
    for (auto& c : str)
        c = (char)tolower(c);
}

inline std::string ToLower(const std::string& str)
{
    std::string ret = str;
    ToLower(ret);
    return ret;
}

inline void ToUpper(std::string& str)
{
    for (auto& c : str)
        c = (char)toupper(c);
}

inline std::string ToUpper(const std::string& str)
{
    std::string ret = str;
    ToUpper(ret);
    return ret;
}

inline std::string GetExtension(const path& p)
{
    return ToLower(p.extension().string());
}

template<class T, class S, class Compare = std::less<T>>
inline static auto InsertSorted(std::vector<T>& cont, S&& val, Compare comp)
{
    auto it = std::upper_bound(cont.begin(), cont.end(), val, comp);
    return cont.insert(it, std::forward<S>(val));
}

template<class T, class V>
inline std::vector<T>::iterator Erase(std::vector<T>& vec, const V& value)
{
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end()) {
        it = vec.erase(it);
    }
    return it;
}

template<class T>
std::span<std::byte> MakeByteSpan(T& obj) {
    return std::span<std::byte>((std::byte*)&obj, sizeof(T));
}

} // utilities
} // mw
