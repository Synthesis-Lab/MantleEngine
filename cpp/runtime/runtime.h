#ifndef MANTLE_RUNTIME_H
#define MANTLE_RUNTIME_H

#include "core/engine.h"
#include "core/time_step.h"
#include "modules/input/input_system.h"
#include "modules/physics/physics_world.h"
#include "modules/sound/sound_system.h"
#include "renderer.h"
#include <memory>

class Runtime {
public:
    Runtime();
    ~Runtime();

    bool Initialize(Renderer::Type renderer_type = Renderer::Type::AUTO);
    void Update(float delta_time);
    void Shutdown();

    InputSystem* Input();
    SoundSystem* Sound();
    PhysicsWorld* Physics();

private:
    Engine engine_;
    TimeStep physics_step_;

    std::unique_ptr<PhysicsWorld> physics_owner_;

    InputSystem* input_ = nullptr;
    SoundSystem* sound_ = nullptr;
    PhysicsWorld* physics_ = nullptr;

    std::unique_ptr<Renderer> renderer_;
    bool initialized_ = false;
};

#endif // MANTLE_RUNTIME_H

