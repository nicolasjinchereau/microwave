/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Object.h>
#include <MW/System/Dispatcher.h>
#include <unordered_map>

namespace mw {

inline namespace scene {
class AudioSource;
}

inline namespace audio {

class AudioContext : public Object
{
protected:
    std::unordered_map<uint32_t, AudioSource*> sources;

    virtual void SetActive() = 0;
    void NotifyBuffersCompleted(AudioSource* source, int bufferCount);

    friend class AudioSource;
public:

    static sptr<AudioContext> New();

    static void SetCurrent(const sptr<AudioContext>& context);
    static sptr<AudioContext> GetCurrent();
};

} // audio
} // mw
