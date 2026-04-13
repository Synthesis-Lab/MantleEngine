#include "mantle_engine.h"
#include <iostream>

int main() {
    MantleEngine engine;

    if (!engine.Initialize(Renderer::Type::AUTO)) {
        std::cerr << "[mantle_demo] Engine initialization failed" << std::endl;
        return 1;
    }

    for (int i = 0; i < 3; ++i) {
        engine.Update(1.0f / 60.0f);
    }

    engine.Shutdown();
    std::cout << "[mantle_demo] Runtime tick completed" << std::endl;
    return 0;
}

