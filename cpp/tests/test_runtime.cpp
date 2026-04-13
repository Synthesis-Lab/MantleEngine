#include <gtest/gtest.h>

#include "runtime.h"

TEST(RuntimeTest, InitializesAndShutsDown) {
    Runtime runtime;

    EXPECT_TRUE(runtime.Initialize(Renderer::Type::AUTO));
    EXPECT_NE(runtime.Input(), nullptr);
    EXPECT_NE(runtime.Sound(), nullptr);
    EXPECT_NE(runtime.Physics(), nullptr);

    runtime.Update(1.0f / 60.0f);
    runtime.Shutdown();
}

TEST(RuntimeTest, PhysicsStepAdvancesBodies) {
    Runtime runtime;
    ASSERT_TRUE(runtime.Initialize(Renderer::Type::AUTO));

    auto* physics = runtime.Physics();
    ASSERT_NE(physics, nullptr);

    Body2D body;
    body.x = 0.0f;
    body.y = 0.0f;
    body.vx = 10.0f;
    body.vy = 0.0f;

    const int id = physics->AddBody(body);
    runtime.Update(1.0f / 30.0f);

    const Body2D* updated = physics->GetBody(id);
    ASSERT_NE(updated, nullptr);
    EXPECT_GT(updated->x, 0.0f);

    runtime.Shutdown();
}

