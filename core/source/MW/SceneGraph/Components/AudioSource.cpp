/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.AudioSource;
import Microwave.Audio.AudioContext;
import Microwave.System.Exception;
import Microwave.System.ThreadPool;
import Microwave.System.Pointers;
import <MW/Audio/Internal/OpenAL.h>;
import <MW/System/Debug.h>;
import <array>;
import <atomic>;
import <cstddef>;
import <cstdint>;
import <mutex>;
import <vector>;

namespace mw {
inline namespace scene {

void StreamController::QueueBuffers(int count)
{
    buffersCompleted += count;
    
    std::lock_guard<std::mutex> lk(mut);

    if (buffersCompleted != 0 && source != 0) {
        ThreadPool::InvokeAsync([this] { ReplenishBuffers(); });
    }
}

void StreamController::Initialize(
    uint32_t sourceID,
    const gptr<AudioClip>& clip)
{
    std::lock_guard<std::mutex> lk(mut);

    source = sourceID;
    bufferFormat = clip->GetBufferFormat();
    sampleRate = clip->GetSampleRate();
    stream = clip->OpenAudioStream();
    data.resize(BufferSize);

    for (auto& buffID : bufferIDs) {
        alGenBuffers(1, &buffID);
    }
}

void StreamController::Terminate()
{
    std::lock_guard<std::mutex> lk(mut);

    source = 0;
    bufferFormat = 0;
    sampleRate = 0;
    stream = nullptr;
    data = std::vector<std::byte>();

    for (auto& buffID : bufferIDs) {
        alDeleteBuffers(1, &buffID);
        buffID = 0;
    }
}

void StreamController::StartBuffering()
{
    std::lock_guard<std::mutex> lk(mut);

    stream->Seek(0, SeekOrigin::Begin);

    int i = 0;

    for (; i != bufferIDs.size(); ++i)
    {
        size_t bytesRead = stream->Read(data);
        if (bytesRead == 0)
            break;

        alBufferData(
            bufferIDs[i],
            bufferFormat,
            data.data(),
            bytesRead,
            sampleRate
        );
    }

    int bufferCount = i;
    alSourceQueueBuffers(source, bufferCount, &bufferIDs[0]);
    buffersCompleted = 0;
}

void StreamController::StopBuffering()
{
    std::lock_guard<std::mutex> lk(mut);

    // source should already be stopped here

    ALint queuedBuffers;
    alGetSourcei(source, AL_BUFFERS_QUEUED, &queuedBuffers);

    if (queuedBuffers != 0)
    {
        std::array<ALuint, BufferCount> buffs;
        alSourceUnqueueBuffers(source, queuedBuffers, &buffs[0]);
    }
}

void StreamController::SetLooping(bool looping)
{
    this->looping = looping;
}

int FetchSubClamp(std::atomic<int>& number, int operand, int min)
{
    int num;
    do { num = number; }
    while (!number.compare_exchange_weak(num, std::max(num - operand, min)));
    return num;
}

void StreamController::ReplenishBuffers()
{
    //int processed;
    //alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

    while (FetchSubClamp(buffersCompleted, 1, 0) > 0)
    {
        std::lock_guard<std::mutex> lk(mut);

        if (source == 0)
            break;

        ALuint bufferID;
        alSourceUnqueueBuffers(source, 1, &bufferID);

        size_t bytesRead = stream->Read(data);
        if (bytesRead == 0)
        {
            if (looping) {
                stream->Seek(0, SeekOrigin::Begin);
                bytesRead = stream->Read(data);
            }
        }

        if (bytesRead != 0)
        {
            alBufferData(
                bufferID,
                bufferFormat,
                data.data(),
                bytesRead,
                sampleRate
            );

            alSourceQueueBuffers(source, 1, &bufferID);
        }

        //alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
    }
}

void AudioSource::Construct()
{
    auto context = AudioContext::GetCurrent();
    if (!context)
        throw Exception("cannot create an audio source with no active audio context");

    // generate an audio source
    alGenSources(1, &source);
    Assert(alIsSource(source));

    alSource3f(source, AL_POSITION, 0.0, 0.0, 0.0f);
    alSource3f(source, AL_VELOCITY, 0.0, 0.0, 0.0);

    ALfloat direction[] = { 0.0, 0.0, 0.0 }; // omnidirectional
    alSourcefv(source, AL_DIRECTION, direction);

    // disable the affect of distance on volume
    alSourcef(source, AL_ROLLOFF_FACTOR, 0.0);
    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

    this->context = context;
    context->sources[source] = this;
}

void AudioSource::Destruct()
{
    if (auto ctx = context.lock())
    {
        SetClip(nullptr);

        if (alIsSource(source))
            alDeleteSources(1, &source);

        ctx->sources.erase(source);
    }

    source = 0;
}

void AudioSource::SetClipBuffer()
{
    Assert(clip->GetBufferID());
    alSourcei(source, AL_BUFFER, clip->GetBufferID());
}

void AudioSource::UnsetClipBuffer()
{
    alSourcei(source, AL_BUFFER, 0);
}

void AudioSource::SetClip(const gptr<AudioClip>& newClip)
{
    if (clip == newClip)
        return;

    if (clip)
    {
        Stop();

        if (clip->IsStreamedFromDisk())
            streamController.Terminate();
        else
            UnsetClipBuffer();
    }

    clip = newClip;

    if (clip)
    {
        if(clip->IsStreamedFromDisk())
            streamController.Initialize(source, clip);
        else
            SetClipBuffer();
    }
}

gptr<AudioClip> AudioSource::GetClip() const {
    return clip;
}

void AudioSource::Play()
{
    if (!clip) return;

    if (IsPlaying())
        Stop();
    
    if (clip->IsStreamedFromDisk())
        streamController.StartBuffering();
    else
        ApplyLoopSetting();

    alSourcePlay(source);
}

void AudioSource::Stop()
{
    if (!clip) return;
    alSourceStop(source);

    if (clip->IsStreamedFromDisk())
        streamController.StopBuffering();
}

void AudioSource::OnBuffersCompleted(int count) {
    streamController.QueueBuffers(count);
}

void AudioSource::Pause() {
    alSourcePause(source);
}

bool AudioSource::IsPlaying() const
{
    ALenum state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

bool AudioSource::IsPaused() const
{
    ALenum state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return (state == AL_PAUSED);
}

bool AudioSource::IsStopped() const
{
    ALenum state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return (state == AL_INITIAL || state == AL_STOPPED);
}

void AudioSource::SetPitch(float pitch) {
    alSourcef(source, AL_PITCH, pitch);
}

float AudioSource::GetPitch() const
{
    float pitch = 0;
    alGetSourcef(source, AL_PITCH, &pitch);
    return pitch;
}

void AudioSource::ApplyLoopSetting() {
    alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
}

void AudioSource::SetLooping(bool loop)
{
    if (loop != looping)
    {
        looping = loop;
        streamController.SetLooping(loop);

        if (!IsStopped() && !clip->IsStreamedFromDisk())
            ApplyLoopSetting();
    }
} 

bool AudioSource::GetLooping() const {
    return looping;
}

void AudioSource::SetVolume(float gain) {
    alSourcef(source, AL_GAIN, gain);
}

float AudioSource::GetVolume() const
{
    float gain = 0;
    alGetSourcef(source, AL_GAIN, &gain);
    return gain;
}

} // scene
} // mw
