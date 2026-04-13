#include "modules/physics/physics_world.h"

bool PhysicsWorld::Initialize() {
    bodies_.clear();
    return true;
}

void PhysicsWorld::Update(float delta_time) {
    for (auto& body : bodies_) {
        body.x += body.vx * delta_time;
        body.y += body.vy * delta_time;
    }
}

void PhysicsWorld::Shutdown() {
    bodies_.clear();
}

const char* PhysicsWorld::GetName() const {
    return "PhysicsWorld";
}

int PhysicsWorld::AddBody(const Body2D& body) {
    bodies_.push_back(body);
    return static_cast<int>(bodies_.size() - 1);
}

const Body2D* PhysicsWorld::GetBody(int index) const {
    if (index < 0 || static_cast<std::size_t>(index) >= bodies_.size()) {
        return nullptr;
    }
    return &bodies_[static_cast<std::size_t>(index)];
}

