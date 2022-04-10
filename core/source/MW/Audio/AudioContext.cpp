/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Audio.AudioContext;
import Microwave.SceneGraph.Components.AudioSource;
import Microwave.System.Pointers;
import Microwave.System.Spinlock;
import <cassert>;
import <memory>;
import <mutex>;

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
