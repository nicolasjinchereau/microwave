/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Audio.AudioContext;
import Microwave.System.Dispatcher;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <cstdint>;
import <unordered_map>;

export namespace mw {

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

    friend AudioSource;
public:

    static sptr<AudioContext> New();

    static void SetCurrent(const sptr<AudioContext>& context);
    static sptr<AudioContext> GetCurrent();
};

} // audio
} // mw
