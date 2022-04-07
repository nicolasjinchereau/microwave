/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Pointers.h>
#include <MW/System/path.h>
#include <type_traits>
#include <string>
#include <optional>
#include <third_party/json/json.hpp>
using json = nlohmann::json;

namespace mw {
inline namespace system {

template<class T>
inline void to_json(json& val, const sptr<T>& object) {
    val = object ? json(*object) : json(nullptr);
}

template<class T>
inline void from_json(const json& obj, sptr<T>& object)
{
    if(!obj.is_null()) {
        object = spnew<T>();
        from_json(obj, *object);
    }
    else {
        object = nullptr;
    }
}

template<class T>
inline void to_json(json& obj, const std::optional<T>& opt) {
    obj = opt ? json(*opt) : json(nullptr);
}

template<class T>
inline void from_json(const json& obj, std::optional<T>& opt) {
    opt = !obj.is_null() ? std::optional<T>(obj.get<T>()) : std::optional<T>();
}

inline void to_json(json& obj, const path& p) {
    obj = p.generic_string();
}

inline void from_json(const json& obj, path& p) {
    p = obj.get<std::string>();
}

} // system

} // mw
