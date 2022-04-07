/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Audio/AudioContext.h>
#include <MW/Scene/Components/AudioSource.h>
#include <MW/System/Spinlock.h>
#include <cassert>
#include <memory>

namespace mw {
inline namespace audio {

Spinlock contextLock;
sptr<AudioContext> currentContext = nullptr;

void AudioContext::SetCurrent(const sptr<AudioContext>& context)
{
    std::lock_guard<Spinlock> lk(contextLock);

    if (context != currentContext)
    {
        if(currentContext = context)
            currentContext->SetActive();
    }
}

sptr<AudioContext> AudioContext::GetCurrent()
{
    std::lock_guard<Spinlock> lk(contextLock);
    return currentContext;
}

void AudioContext::NotifyBuffersCompleted(AudioSource* source, int bufferCount)
{
    assert(source);
    source->OnBuffersCompleted(bufferCount);
}

} // audio
} // mw
