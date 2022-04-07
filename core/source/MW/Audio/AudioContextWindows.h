/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Audio/AudioContext.h>
#include <AL/al.h>
#include <AL/alc.h>

namespace mw {
inline namespace audio {

class AudioContextWindows : public AudioContext
{
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
protected:
    virtual void SetActive() override;
public:
    AudioContextWindows();
    ~AudioContextWindows();

    static void DispatchEvent(
        ALenum eventType,
        ALuint object,
        ALuint param,
        ALsizei messageLength,
        const ALchar* message,
        void* userParam);

    void ProcessEvent(ALenum eventType, ALuint object, ALuint param);
};

} // audio
} // mw
