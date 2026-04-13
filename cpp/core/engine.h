#ifndef MANTLE_CORE_ENGINE_H
#define MANTLE_CORE_ENGINE_H

#include "module.h"
#include <memory>
#include <vector>

class Engine {
public:
    Engine() = default;

    void RegisterModule(std::unique_ptr<Module> module);
    bool Initialize();
    void Update(float delta_time);
    void Shutdown();

private:
    std::vector<std::unique_ptr<Module>> modules_;
    bool initialized_ = false;
};

#endif // MANTLE_CORE_ENGINE_H

