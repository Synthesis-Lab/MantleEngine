#ifndef MANTLE_PHYSICS_WORLD_H
#define MANTLE_PHYSICS_WORLD_H

#include "core/module.h"
#include <vector>

struct Body2D {
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
};

class PhysicsWorld final : public Module {
public:
    bool Initialize() override;
    void Update(float delta_time) override;
    void Shutdown() override;
    const char* GetName() const override;

    int AddBody(const Body2D& body);
    const Body2D* GetBody(int index) const;

private:
    std::vector<Body2D> bodies_;
};

#endif // MANTLE_PHYSICS_WORLD_H

