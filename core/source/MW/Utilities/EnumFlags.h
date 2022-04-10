/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

namespace detail {
typedef int enum_t;
}

#define ENUM_FLAGS(e) \
inline e operator|(e left, e right) { return (e)((detail::enum_t)left | (detail::enum_t)right); } \
inline e operator&(e left, e right) { return (e)((detail::enum_t)left & (detail::enum_t)right); } \
inline detail::enum_t operator&(e left, detail::enum_t right) { return (detail::enum_t)left & right; } \
inline e operator^(e left, e right) { return (e)((detail::enum_t)left ^ (detail::enum_t)right); } \
inline e operator~(e value) { return (e)(~(detail::enum_t)value); } \
inline e& operator|=(e& left, e right) { return (left = (left | right)); } \
inline e& operator&=(e& left, e right) { return (left = (left & right)); } \
inline e& operator^=(e& left, e right) { return (left = (left ^ right)); } \
inline e operator+(e left, detail::enum_t right) { return (e)((detail::enum_t)left + right); } \
inline e operator-(e left, detail::enum_t right) { return (e)((detail::enum_t)left - right); } \
inline e operator+(e left, e right) { return (e)((detail::enum_t)left + (detail::enum_t)right); } \
inline e operator-(e left, e right) { return (e)((detail::enum_t)left - (detail::enum_t)right); } \
inline e& operator+=(e& left, detail::enum_t right) { return (left = (e)((detail::enum_t)left + right)); } \
inline e& operator-=(e& left, detail::enum_t right) { return (left = (e)((detail::enum_t)left - right)); } \
inline e& operator+=(e& left, e right) { return (left = (e)((detail::enum_t)left + (detail::enum_t)right)); } \
inline e& operator-=(e& left, e right) { return (left = (e)((detail::enum_t)left - (detail::enum_t)right)); } \
inline bool operator==(e left, detail::enum_t right) { return (detail::enum_t)left == right; } \
inline bool operator!=(e left, detail::enum_t right) { return (detail::enum_t)left != right; } \
inline bool operator<(detail::enum_t left, e right) { return left < (detail::enum_t)right; } \
inline bool operator<=(detail::enum_t left, e right) { return left <= (detail::enum_t)right; } \
inline bool operator>(detail::enum_t left, e right) { return left > (detail::enum_t)right; } \
inline bool operator>=(detail::enum_t left, e right) { return left >= (detail::enum_t)right; } \
inline bool operator<(e left, detail::enum_t right) { return (detail::enum_t)left < right; } \
inline bool operator<=(e left, detail::enum_t right) { return (detail::enum_t)left <= right; } \
inline bool operator>(e left, detail::enum_t right) { return (detail::enum_t)left > right; } \
inline bool operator>=(e left, detail::enum_t right) { return (detail::enum_t)left >= right; } \
inline detail::enum_t operator*(e val) { return (detail::enum_t)val; }
