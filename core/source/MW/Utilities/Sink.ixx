/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Utilities.Sink;
import Microwave.System.Pointers;
import <vector>;
import <utility>;

export namespace mw {
inline namespace utilities {

template<class T>
class Sink
{
    gvector<T>& cont;
public:
    Sink(gvector<T>& cont) : cont(cont) {}

    template<class U>
    void Add(U&& value) {
        cont.push_back(std::forward<U>(value));
    }
};

} // utilities
} // mw
