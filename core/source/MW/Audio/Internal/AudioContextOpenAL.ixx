/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Audio.Internal.AudioContextOpenAL;
import Microwave.Audio.AudioContext;
import <MW/Audio/Internal/OpenAL.h>;

export namespace mw {
inline namespace audio {

class AudioContextOpenAL : public AudioContext
{
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
protected:
    virtual void SetActive() override;
public:
    AudioContextOpenAL();
    ~AudioContextOpenAL();

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