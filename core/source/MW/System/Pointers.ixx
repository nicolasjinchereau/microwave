/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Pointers;
import <deque>;
import <functional>;
import <list>;
import <memory>;
import <unordered_map>;
import <unordered_set>;
import <vector>;
import <gc/gc.h>;

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

template<class T>
using gptr = gc::graph_ptr<T>;

template<class T>
using wgptr = gc::weak_graph_ptr<T>;

template<class T>
using rgptr = gc::raw_graph_ptr<T>;

template<typename T, typename... Args> requires std::negation_v<std::is_array<T>>
gptr<T> gpnew(Args&&... args) { return gc::graph::make<T>(std::forward<Args>(args)...); }

template<class To, class From>
gptr<To> gpcast(const gptr<From>& ptr) noexcept { return gc::dynamic_pointer_cast<To>(ptr); }

template<class To, class From>
gptr<To> gpcast(gptr<From>&& ptr) noexcept { return gc::dynamic_pointer_cast<To>(std::move(ptr)); }

template<class T>
using gvector = std::vector<T, gc::allocator<T>>;

template<class T>
using glist = std::list<T, gc::allocator<T>>;

template<class T, class Comp = std::less<T>>
using gpriority_queue = std::priority_queue<T, std::vector<T, gc::allocator<T>>, Comp>;

template<class K, class H = std::hash<K>, class E = std::equal_to<K>>
using gset = std::unordered_set<K, H, E, gc::allocator<K>>;

template<class K, class T, class H = std::hash<K>, class E = std::equal_to<K>>
using gmap = std::unordered_map<K, T, H, E, gc::allocator<std::pair<const K, T>>>;

template<class F>
using gfunction = gc::function<F>;

} // system
} // mw
