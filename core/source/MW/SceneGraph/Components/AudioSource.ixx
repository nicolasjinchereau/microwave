/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.AudioSource;
import Microwave.SceneGraph.Components.Component;
import Microwave.Audio.AudioClip;
import Microwave.Audio.AudioStream;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <array>;
import <atomic>;
import <cstddef>;
import <cstdint>;
import <mutex>;
import <vector>;

export namespace mw {

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
    uint32_t source = 0;
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
    inline static Type::Pin<AudioSource> pin;

    uint32_t source = 0;
    sptr<AudioClip> clip;
    wptr<AudioContext> context;
    bool looping = false;
    StreamController streamController;

    void OnBuffersCompleted(int count);
    friend AudioContext;

    void Construct();
    void Destruct();
public:

    AudioSource() { Construct(); }
    ~AudioSource() { Destruct(); }

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
