/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Clock;
import <chrono>;

export namespace mw {
inline namespace system {

class Clock
{
    using clock_type = std::chrono::high_resolution_clock;
    using float_duration = std::chrono::duration<float>;
    clock_type::time_point startTime;
    clock_type::time_point lastTickTime;
    clock_type::duration deltaTime;
    clock_type::duration maxDeltaTime;
public:
    Clock() {
        maxDeltaTime = std::chrono::duration_cast<clock_type::duration>(float_duration(0.1f));
        Reset();
    }

    void Reset() {
        startTime = clock_type::now();
        lastTickTime = startTime;
        deltaTime = std::chrono::seconds(0);
    }

    void Tick() {
        auto now = clock_type::now();
        deltaTime = (now - lastTickTime);
        lastTickTime = now;
    }

    float GetMaxDeltaTime() const {
        using std::chrono::duration_cast;
        return duration_cast<float_duration>(maxDeltaTime).count();
    }

    void SetMaxDeltaTime(float deltaTime) {
        using std::chrono::duration_cast;
        maxDeltaTime = duration_cast<clock_type::duration>(float_duration(deltaTime));
    }

    float GetTime() const {
        auto elapsed = clock_type::now() - startTime;
        auto seconds = std::chrono::duration_cast<float_duration>(elapsed).count();
        return seconds;
    }

    float GetDeltaTime() const {
        auto dt = std::min(deltaTime, maxDeltaTime);
        return std::chrono::duration_cast<float_duration>(dt).count();
    }
};

} // system
} // mw
