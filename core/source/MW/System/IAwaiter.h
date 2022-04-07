/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once

namespace mw {
inline namespace system {

class IAwaiter
{
public:
    virtual ~IAwaiter() = default;
    virtual void Resume() = 0;
};

} // system
} // mw
