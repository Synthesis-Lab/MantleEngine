#ifndef MANTLE_CORE_TIME_STEP_H
#define MANTLE_CORE_TIME_STEP_H

#include <algorithm>

class TimeStep {
public:
    TimeStep(float fixed_step = 1.0f / 60.0f, float max_step = 0.1f)
        : fixed_step_(fixed_step), max_step_(max_step), accumulator_(0.0f) {}

    void AddFrameTime(float delta_time) {
        accumulator_ += std::clamp(delta_time, 0.0f, max_step_);
    }

    bool HasStep() const {
        return accumulator_ >= fixed_step_;
    }

    float ConsumeStep() {
        accumulator_ -= fixed_step_;
        return fixed_step_;
    }

private:
    float fixed_step_;
    float max_step_;
    float accumulator_;
};

#endif // MANTLE_CORE_TIME_STEP_H

