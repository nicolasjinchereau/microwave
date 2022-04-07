/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <type_traits>
#include <functional>

#define ENUM_FLAGS(e) \
inline e operator|(e left, e right) { return (e)((std::underlying_type<e>::type)left | (std::underlying_type<e>::type)right); } \
inline e operator&(e left, e right) { return (e)((std::underlying_type<e>::type)left & (std::underlying_type<e>::type)right); } \
inline std::underlying_type<e>::type operator&(e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left & right; } \
inline e operator^(e left, e right) { return (e)((std::underlying_type<e>::type)left ^ (std::underlying_type<e>::type)right); } \
inline e operator~(e value) { return (e)(~(std::underlying_type<e>::type)value); } \
inline e& operator|=(e& left, e right) { return (left = (left | right)); } \
inline e& operator&=(e& left, e right) { return (left = (left & right)); } \
inline e& operator^=(e& left, e right) { return (left = (left ^ right)); } \
inline e operator+(e left, std::underlying_type<e>::type right) { return (e)((std::underlying_type<e>::type)left + right); } \
inline e operator-(e left, std::underlying_type<e>::type right) { return (e)((std::underlying_type<e>::type)left - right); } \
inline e operator+(e left, e right) { return (e)((std::underlying_type<e>::type)left + (std::underlying_type<e>::type)right); } \
inline e operator-(e left, e right) { return (e)((std::underlying_type<e>::type)left - (std::underlying_type<e>::type)right); } \
inline e& operator+=(e& left, std::underlying_type<e>::type right) { return (left = (e)((std::underlying_type<e>::type)left + right)); } \
inline e& operator-=(e& left, std::underlying_type<e>::type right) { return (left = (e)((std::underlying_type<e>::type)left - right)); } \
inline e& operator+=(e& left, e right) { return (left = (e)((std::underlying_type<e>::type)left + (std::underlying_type<e>::type)right)); } \
inline e& operator-=(e& left, e right) { return (left = (e)((std::underlying_type<e>::type)left - (std::underlying_type<e>::type)right)); } \
inline bool operator==(e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left == right; } \
inline bool operator!=(e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left != right; } \
inline bool operator< (e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left <  right; } \
inline bool operator<=(e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left <= right; } \
inline bool operator> (e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left >  right; } \
inline bool operator>=(e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left >= right; } \
inline bool operator==(std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left == right; } \
inline bool operator!=(std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left != right; } \
inline bool operator< (std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left <  right; } \
inline bool operator<=(std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left <= right; } \
inline bool operator> (std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left >  right; } \
inline bool operator>=(std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left >= right; } \
inline std::underlying_type<e>::type operator*(e val) { return (std::underlying_type<e>::type)val; }

namespace mw {
inline namespace utilities {

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
