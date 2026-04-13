#include "runtime/runtime.h"

Runtime::Runtime()
    : physics_step_(1.0f / 60.0f, 0.1f) {
}

Runtime::~Runtime() {
    Shutdown();
}

bool Runtime::Initialize(Renderer::Type renderer_type) {
    if (initialized_) {
        return true;
    }

    auto input = std::make_unique<InputSystem>();
    auto sound = std::make_unique<SoundSystem>();
    auto physics = std::make_unique<PhysicsWorld>();

    input_ = input.get();
    sound_ = sound.get();
    physics_ = physics.get();
    physics_owner_ = std::move(physics);

    engine_.RegisterModule(std::move(input));
    engine_.RegisterModule(std::move(sound));

    if (!engine_.Initialize()) {
        input_ = nullptr;
        sound_ = nullptr;
        physics_ = nullptr;
        physics_owner_.reset();
        return false;
    }

    if (physics_ != nullptr && !physics_->Initialize()) {
        engine_.Shutdown();
        input_ = nullptr;
        sound_ = nullptr;
        physics_ = nullptr;
        physics_owner_.reset();
        return false;
    }

    renderer_ = Renderer::Create(renderer_type);
    if (renderer_ != nullptr) {
        if (!renderer_->Initialize()) {
            if (physics_ != nullptr) {
                physics_->Shutdown();
            }
            engine_.Shutdown();
            input_ = nullptr;
            sound_ = nullptr;
            physics_ = nullptr;
            physics_owner_.reset();
            renderer_.reset();
            return false;
        }
    }

    initialized_ = true;
    return true;
}

void Runtime::Update(float delta_time) {
    if (!initialized_) {
        return;
    }

    engine_.Update(delta_time);

    physics_step_.AddFrameTime(delta_time);
    while (physics_step_.HasStep()) {
        const float fixed = physics_step_.ConsumeStep();
        if (physics_ != nullptr) {
            physics_->Update(fixed);
        }
    }
}

void Runtime::Shutdown() {
    if (!initialized_) {
        return;
    }

    if (renderer_ != nullptr) {
        renderer_->Shutdown();
        renderer_.reset();
    }

    if (physics_ != nullptr) {
        physics_->Shutdown();
        physics_owner_.reset();
        physics_ = nullptr;
    }

    engine_.Shutdown();
    input_ = nullptr;
    sound_ = nullptr;
    initialized_ = false;
}

InputSystem* Runtime::Input() {
    return input_;
}

SoundSystem* Runtime::Sound() {
    return sound_;
}

PhysicsWorld* Runtime::Physics() {
    return physics_;
}

