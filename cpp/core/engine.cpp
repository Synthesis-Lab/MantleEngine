#include "core/engine.h"

void Engine::RegisterModule(std::unique_ptr<Module> module) {
    if (!module) {
        return;
    }
    modules_.push_back(std::move(module));
}

bool Engine::Initialize() {
    if (initialized_) {
        return true;
    }

    for (const auto& module : modules_) {
        if (!module->Initialize()) {
            return false;
        }
    }

    initialized_ = true;
    return true;
}

void Engine::Update(float delta_time) {
    if (!initialized_) {
        return;
    }

    for (const auto& module : modules_) {
        module->Update(delta_time);
    }
}

void Engine::Shutdown() {
    if (!initialized_) {
        return;
    }

    for (auto it = modules_.rbegin(); it != modules_.rend(); ++it) {
        (*it)->Shutdown();
    }

    initialized_ = false;
}

