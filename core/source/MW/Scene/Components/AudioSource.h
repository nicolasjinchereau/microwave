/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Audio/AudioClip.h>
#include <MW/Scene/Components/Component.h>

namespace mw {

inline namespace audio {
class AudioContext;
}

inline namespace scene {

class StreamController
{
    void ReplenishBuffers();
public:
    constexpr static int BufferCount = 4;
    constexpr static int BufferSize = 8192;

    std::mutex mut;
    uint32_t source;
    int bufferFormat = 0;
    int sampleRate = 0;
    sptr<AudioStream> stream;
    std::atomic<bool> looping;
    std::atomic<int> buffersCompleted;

    std::array<uint32_t, BufferCount> bufferIDs;
    std::vector<std::byte> data;
    
    void QueueBuffers(int count);

    void Initialize(uint32_t source, const sptr<AudioClip>& clip);
    void Terminate();

    void StartBuffering();
    void StopBuffering();

    void SetLooping(bool looping);
};

class AudioSource : public Component
{
    REGISTER_OBJECT(AudioSource);

    uint32_t source;
    sptr<AudioClip> clip;
    wptr<AudioContext> context;
    bool looping = false;
    StreamController streamController;

    void OnBuffersCompleted(int count);
    friend class AudioContext;
public:

    AudioSource();
    ~AudioSource();

    void SetClip(const sptr<AudioClip>& clip);
    sptr<AudioClip> GetClip() const;

    void Play();
    void Stop();
    void Pause();

    bool IsPlaying() const;
    bool IsPaused() const;
    bool IsStopped() const;

    // default: 1.0
    // range: [0.5 - 2.0]
    void SetPitch(float pitch);
    float GetPitch() const;

    // default: false
    // options: true, false
    void SetLooping(bool loop);
    bool GetLooping() const;

    // default: 1.0
    // range: [0.0 - FLOAT_MAX]
    void SetVolume(float gain);
    float GetVolume() const;

private:

    void SetClipBuffer();
    void UnsetClipBuffer();

    void ApplyLoopSetting();
};

} // scene
} // mw
