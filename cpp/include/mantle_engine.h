#ifndef MANTLE_ENGINE_H
#define MANTLE_ENGINE_H

#include "runtime.h"

class MantleEngine {
public:
    MantleEngine() = default;

    bool Initialize(Renderer::Type renderer_type = Renderer::Type::AUTO) {
        return runtime_.Initialize(renderer_type);
    }

    void Update(float delta_time) {
        runtime_.Update(delta_time);
    }

    void Shutdown() {
        runtime_.Shutdown();
    }

    Runtime& GetRuntime() {
        return runtime_;
    }

private:
    Runtime runtime_;
};

#endif // MANTLE_ENGINE_H

