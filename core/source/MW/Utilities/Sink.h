/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <vector>

namespace mw {
inline namespace utilities {

template<class T>
class Sink
{
    std::vector<T>& cont;
public:
    Sink(std::vector<T>& cont) : cont(cont) {}

    template<class U>
    void Add(U&& value) {
        cont.push_back(std::forward<U>(value));
    }
};

} // utilities
} // mw
