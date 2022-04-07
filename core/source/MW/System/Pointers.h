/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <memory>
#include <cassert>

namespace mw {
inline namespace system {

template<class T>
using uptr = std::unique_ptr<T>;

template<class T>
using sptr = std::shared_ptr<T>;

template<class T>
using wptr = std::weak_ptr<T>;

template<class T>
using sp_from_this = std::enable_shared_from_this<T>;

template<typename T, typename... Args>
inline uptr<T> upnew(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
inline sptr<T> spnew(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

} // system
} // mw
