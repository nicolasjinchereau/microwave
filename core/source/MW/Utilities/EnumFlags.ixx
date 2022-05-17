/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Utilities.EnumFlags;
import <type_traits>;

export namespace mw {
inline namespace utilities {

template<class T>
concept IsEnumFlags = requires {
    std::is_enum_v<T>;
    EnableEnumFlags(T());
};

template<class T> requires IsEnumFlags<T> inline constexpr T operator|(T left, T right) { return static_cast<T>(static_cast<std::underlying_type_t<T>>(left) | static_cast<std::underlying_type_t<T>>(right)); }
template<class T> requires IsEnumFlags<T> inline constexpr T operator&(T left, T right) { return static_cast<T>(static_cast<std::underlying_type_t<T>>(left) & static_cast<std::underlying_type_t<T>>(right)); }
template<class T> requires IsEnumFlags<T> inline constexpr std::underlying_type_t<T> operator&(T left, std::underlying_type_t<T> right) { return static_cast<std::underlying_type_t<T>>(left) & right; }
template<class T> requires IsEnumFlags<T> inline constexpr T operator^(T left, T right) { return static_cast<T>(static_cast<std::underlying_type_t<T>>(left) ^ static_cast<std::underlying_type_t<T>>(right)); }
template<class T> requires IsEnumFlags<T> inline constexpr T operator~(T value) { return static_cast<T>(~(std::underlying_type_t<T>)value); }
template<class T> requires IsEnumFlags<T> inline constexpr T& operator|=(T& left, T right) { return (left = (left | right)); }
template<class T> requires IsEnumFlags<T> inline constexpr T& operator&=(T& left, T right) { return (left = (left & right)); }
template<class T> requires IsEnumFlags<T> inline constexpr T& operator^=(T& left, T right) { return (left = (left ^ right)); }
template<class T> requires IsEnumFlags<T> inline constexpr T operator+(T left, std::underlying_type_t<T> right) { return static_cast<T>(static_cast<std::underlying_type_t<T>>(left) + right); }
template<class T> requires IsEnumFlags<T> inline constexpr T operator-(T left, std::underlying_type_t<T> right) { return static_cast<T>(static_cast<std::underlying_type_t<T>>(left) - right); }
template<class T> requires IsEnumFlags<T> inline constexpr T operator+(T left, T right) { return static_cast<T>(static_cast<std::underlying_type_t<T>>(left) + static_cast<std::underlying_type_t<T>>(right)); }
template<class T> requires IsEnumFlags<T> inline constexpr T operator-(T left, T right) { return static_cast<T>(static_cast<std::underlying_type_t<T>>(left) - static_cast<std::underlying_type_t<T>>(right)); }
template<class T> requires IsEnumFlags<T> inline constexpr T& operator+=(T& left, std::underlying_type_t<T> right) { return (left = static_cast<T>(static_cast<std::underlying_type_t<T>>(left) + right)); }
template<class T> requires IsEnumFlags<T> inline constexpr T& operator-=(T& left, std::underlying_type_t<T> right) { return (left = static_cast<T>(static_cast<std::underlying_type_t<T>>(left) - right)); }
template<class T> requires IsEnumFlags<T> inline constexpr T& operator+=(T& left, T right) { return (left = static_cast<T>(static_cast<std::underlying_type_t<T>>(left) + static_cast<std::underlying_type_t<T>>(right))); }
template<class T> requires IsEnumFlags<T> inline constexpr T& operator-=(T& left, T right) { return (left = static_cast<T>(static_cast<std::underlying_type_t<T>>(left) - static_cast<std::underlying_type_t<T>>(right))); }
template<class T> requires IsEnumFlags<T> inline constexpr bool operator==(T left, std::underlying_type_t<T> right) { return static_cast<std::underlying_type_t<T>>(left) == right; }
template<class T> requires IsEnumFlags<T> inline constexpr bool operator!=(T left, std::underlying_type_t<T> right) { return static_cast<std::underlying_type_t<T>>(left) != right; }
template<class T> requires IsEnumFlags<T> inline constexpr bool operator<(std::underlying_type_t<T> left, T right) { return left < static_cast<std::underlying_type_t<T>>(right); }
template<class T> requires IsEnumFlags<T> inline constexpr bool operator<=(std::underlying_type_t<T> left, T right) { return left <= static_cast<std::underlying_type_t<T>>(right); }
template<class T> requires IsEnumFlags<T> inline constexpr bool operator>(std::underlying_type_t<T> left, T right) { return left > static_cast<std::underlying_type_t<T>>(right); }
template<class T> requires IsEnumFlags<T> inline constexpr bool operator>=(std::underlying_type_t<T> left, T right) { return left >= static_cast<std::underlying_type_t<T>>(right); }
template<class T> requires IsEnumFlags<T> inline constexpr bool operator<(T left, std::underlying_type_t<T> right) { return static_cast<std::underlying_type_t<T>>(left) < right; }
template<class T> requires IsEnumFlags<T> inline constexpr bool operator<=(T left, std::underlying_type_t<T> right) { return static_cast<std::underlying_type_t<T>>(left) <= right; }
template<class T> requires IsEnumFlags<T> inline constexpr bool operator>(T left, std::underlying_type_t<T> right) { return static_cast<std::underlying_type_t<T>>(left) > right; }
template<class T> requires IsEnumFlags<T> inline constexpr bool operator>=(T left, std::underlying_type_t<T> right) { return static_cast<std::underlying_type_t<T>>(left) >= right; }
template<class T> requires IsEnumFlags<T> inline constexpr std::underlying_type_t<T> operator*(T val) { return static_cast<std::underlying_type_t<T>>(val); }

// constexpr void EnableEnumFlags(MyEnumType);

} // utilities
} // mw
