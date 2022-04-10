/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Audio.Internal.AudioContextWindows;
import Microwave.Audio.AudioContext;
import Microwave.SceneGraph.Components.AudioSource;
import Microwave.System.Pointers;
import Microwave.Utilities.Format;
import <cassert>;
import <MW/Audio/Internal/OpenAL.h>;

namespace mw {
inline namespace audio {

AudioContextWindows::AudioContextWindows()
{
    device = alcOpenDevice(NULL);
    if (!device)
        throw FormattedError("error {}: failed to create OpenAL device", (int)alGetError());

    context = alcCreateContext(device, NULL);
    if (!device)
        throw FormattedError("error {}: failed to create OpenAL context", (int)alGetError());

    auto oldContext = alcGetCurrentContext();
    alcMakeContextCurrent(context);

    ALenum event_type = AL_EVENT_TYPE_BUFFER_COMPLETED_SOFT;
    alEventControlSOFT(1, &event_type, AL_TRUE);
    alEventCallbackSOFT(DispatchEvent, this);

    alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
    alListener3f(AL_VELOCITY, 0.0, 0.0, 0.0);
    ALfloat orientation[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
    alListenerfv(AL_ORIENTATION, orientation);

    alcMakeContextCurrent(oldContext);
}

AudioContextWindows::~AudioContextWindows()
{
    if (context)
        alcDestroyContext(context);

    if (device)
        alcCloseDevice(device);

    context = nullptr;
    device = nullptr;
}

void AudioContextWindows::SetActive() {
    alcMakeContextCurrent(context);
}

void AudioContextWindows::DispatchEvent(
    ALenum eventType,
    ALuint object,
    ALuint param,
    ALsizei messageLength,
    const ALchar* message,
    void* userParam)
{
    auto context = (AudioContextWindows*)userParam;
    context->ProcessEvent(eventType, object, param);
}

void AudioContextWindows::ProcessEvent(ALenum eventType, ALuint object, ALuint param)
{
    if (eventType == AL_EVENT_TYPE_BUFFER_COMPLETED_SOFT)
    {
        ALuint sourceID = object;
        int bufferCount = (int)param;

        auto it = sources.find(sourceID);
        if (it != sources.end())
            NotifyBuffersCompleted(it->second, bufferCount);
    }
}

sptr<AudioContext> AudioContext::New()
{
    return spnew<AudioContextWindows>();
}

} // audio
} // mw
