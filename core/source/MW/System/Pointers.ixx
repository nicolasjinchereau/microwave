/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Pointers;
import <memory>;
import <cassert>;

export namespace mw {
inline namespace system {

template<class T, class Deleter = std::default_delete<T>>
using uptr = std::unique_ptr<T, Deleter>;

template<class T>
using sptr = std::shared_ptr<T>;

template<class T>
using wptr = std::weak_ptr<T>;

template<class T>
using sp_from_this = std::enable_shared_from_this<T>;

template<typename T, typename... Args>
uptr<T> upnew(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
sptr<T> spnew(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<class T1, class T2>
sptr<T1> spcast(const sptr<T2>& other) noexcept
{
    const auto p = dynamic_cast<typename sptr<T1>::element_type*>(other.get());
    if (p) return sptr<T1>(other, p);
    return {};
}

template<class T1, class T2>
sptr<T1> spcast(sptr<T2>&& other) noexcept
{
    const auto p = dynamic_cast<typename sptr<T1>::element_type*>(other.get());
    if (p) return sptr<T1>(std::move(other), p);
    return {};
}

} // system
} // mw
