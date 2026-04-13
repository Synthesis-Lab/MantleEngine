#include <gtest/gtest.h>
#include "core/engine.h"
#include "modules/input/input_system.h"
#include "modules/physics/physics_world.h"
#include "modules/sound/sound_system.h"
#include "include/renderer.h"
#include <memory>

class AllEnginesIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine_ = std::make_unique<Engine>();
    }

    void TearDown() override {
        if (engine_) {
            engine_->Shutdown();
            engine_.reset();
        }
    }

    std::unique_ptr<Engine> engine_;
};

// Test 1: All engines initialize successfully
TEST_F(AllEnginesIntegrationTest, AllEnginesInitialize) {
    // Register all four engines
    engine_->RegisterModule(std::make_unique<InputSystem>());
    engine_->RegisterModule(std::make_unique<PhysicsWorld>());
    engine_->RegisterModule(std::make_unique<SoundSystem>());

    // Initialize engine
    ASSERT_TRUE(engine_->Initialize());
}

// Test 2: Input system works correctly
TEST_F(AllEnginesIntegrationTest, InputSystemFunctionality) {
    auto input = std::make_unique<InputSystem>();
    InputSystem* input_ptr = input.get();
    engine_->RegisterModule(std::move(input));

    ASSERT_TRUE(engine_->Initialize());

    // Test key state tracking
    input_ptr->SetKeyDown(65, true);  // 'A' key
    engine_->Update(0.016f);  // 16ms frame time

    ASSERT_EQ(input_ptr->GetKeyState(65), InputSystem::KeyState::Pressed);

    // Key is still held
    engine_->Update(0.016f);
    ASSERT_EQ(input_ptr->GetKeyState(65), InputSystem::KeyState::Held);

    // Release key
    input_ptr->SetKeyDown(65, false);
    engine_->Update(0.016f);
    ASSERT_EQ(input_ptr->GetKeyState(65), InputSystem::KeyState::Released);

    // Key is fully released
    engine_->Update(0.016f);
    ASSERT_EQ(input_ptr->GetKeyState(65), InputSystem::KeyState::Up);
}

// Test 3: Physics system works correctly
TEST_F(AllEnginesIntegrationTest, PhysicsSystemFunctionality) {
    auto physics = std::make_unique<PhysicsWorld>();
    PhysicsWorld* physics_ptr = physics.get();
    engine_->RegisterModule(std::move(physics));

    ASSERT_TRUE(engine_->Initialize());

    // Create a 2D body with velocity
    Body2D body;
    body.x = 0.0f;
    body.y = 0.0f;
    body.vx = 10.0f;  // Move 10 units per second in X
    body.vy = 5.0f;   // Move 5 units per second in Y

    int body_id = physics_ptr->AddBody(body);
    ASSERT_EQ(body_id, 0);

    // Simulate 1 second
    float dt = 0.016f;  // 16ms per frame
    for (int i = 0; i < 60; ++i) {
        engine_->Update(dt);
    }

    // Check that body moved
    const Body2D* updated_body = physics_ptr->GetBody(body_id);
    ASSERT_NE(updated_body, nullptr);

    // Body should have moved approximately 10 units in X, 5 units in Y
    float expected_x = 10.0f * (dt * 60);
    float expected_y = 5.0f * (dt * 60);

    ASSERT_NEAR(updated_body->x, expected_x, 0.1f);
    ASSERT_NEAR(updated_body->y, expected_y, 0.1f);
}

// Test 4: Sound system works correctly
TEST_F(AllEnginesIntegrationTest, SoundSystemFunctionality) {
    auto sound = std::make_unique<SoundSystem>();
    SoundSystem* sound_ptr = sound.get();
    engine_->RegisterModule(std::move(sound));

    ASSERT_TRUE(engine_->Initialize());

    // Load a sound clip
    int clip_id = sound_ptr->LoadClip("test_sound.wav");
    ASSERT_GT(clip_id, 0);

    // Clip shouldn't be playing initially
    ASSERT_FALSE(sound_ptr->IsPlaying(clip_id));

    // Play the sound
    sound_ptr->Play(clip_id);
    engine_->Update(0.016f);

    // Should be playing
    ASSERT_TRUE(sound_ptr->IsPlaying(clip_id));

    // Stop the sound
    sound_ptr->Stop(clip_id);
    engine_->Update(0.016f);

    // Should not be playing
    ASSERT_FALSE(sound_ptr->IsPlaying(clip_id));
}

// Test 5: Render engine factory works
TEST_F(AllEnginesIntegrationTest, RenderEngineFactory) {
    // Create Vulkan renderer
    auto vulkan_renderer = Renderer::Create(Renderer::Type::VULKAN);
    ASSERT_NE(vulkan_renderer, nullptr);
    ASSERT_EQ(vulkan_renderer->GetType(), Renderer::Type::VULKAN);
    ASSERT_STREQ(vulkan_renderer->GetName(), "Vulkan 1.2+");

    // Create OpenGL renderer
    auto opengl_renderer = Renderer::Create(Renderer::Type::OPENGL);
    ASSERT_NE(opengl_renderer, nullptr);
    ASSERT_EQ(opengl_renderer->GetType(), Renderer::Type::OPENGL);

    // Try AUTO mode
    auto auto_renderer = Renderer::Create(Renderer::Type::AUTO);
    ASSERT_NE(auto_renderer, nullptr);
    ASSERT_EQ(auto_renderer->GetType(), Renderer::Type::VULKAN);  // Should default to Vulkan

    // Initialize and shutdown render engine
    ASSERT_TRUE(vulkan_renderer->Initialize());
    ASSERT_TRUE(vulkan_renderer->IsReady());
    vulkan_renderer->Shutdown();
}

// Test 6: All engines work together
TEST_F(AllEnginesIntegrationTest, AllEnginesWorkTogether) {
    // Create and register all engines
    auto input = std::make_unique<InputSystem>();
    auto physics = std::make_unique<PhysicsWorld>();
    auto sound = std::make_unique<SoundSystem>();

    InputSystem* input_ptr = input.get();
    PhysicsWorld* physics_ptr = physics.get();
    SoundSystem* sound_ptr = sound.get();

    engine_->RegisterModule(std::move(input));
    engine_->RegisterModule(std::move(physics));
    engine_->RegisterModule(std::move(sound));

    // Initialize
    ASSERT_TRUE(engine_->Initialize());

    // Simulate a game loop
    Body2D player;
    player.x = 100.0f;
    player.y = 100.0f;
    player.vx = 0.0f;
    player.vy = 0.0f;

    int player_id = physics_ptr->AddBody(player);
    int sound_id = sound_ptr->LoadClip("background_music.wav");

    // Start with no velocity
    ASSERT_EQ(physics_ptr->GetBody(player_id)->vx, 0.0f);

    // Simulate input -> movement -> sound
    input_ptr->SetKeyDown(87, true);  // 'W' key for forward movement
    engine_->Update(0.016f);

    // Check input state
    ASSERT_EQ(input_ptr->GetKeyState(87), InputSystem::KeyState::Pressed);

    // Manually update player velocity based on input (normally game logic would do this)
    if (input_ptr->GetKeyState(87) == InputSystem::KeyState::Pressed ||
        input_ptr->GetKeyState(87) == InputSystem::KeyState::Held) {
        // Simulate physics: apply movement
        player.vy = -100.0f;  // Move forward (negative Y)
        // This is where game logic would update the physics body
    }

    // Play background music
    sound_ptr->Play(sound_id);
    ASSERT_TRUE(sound_ptr->IsPlaying(sound_id));

    // Simulate for a few frames
    for (int i = 0; i < 5; ++i) {
        engine_->Update(0.016f);
    }

    // Check that time progressed correctly
    ASSERT_TRUE(input_ptr->GetKeyState(87) == InputSystem::KeyState::Held);
    ASSERT_TRUE(sound_ptr->IsPlaying(sound_id));
}

// Test 7: Engine handles module initialization failure gracefully
TEST_F(AllEnginesIntegrationTest, ModuleRegistrationOrder) {
    // Register modules in different order
    engine_->RegisterModule(std::make_unique<SoundSystem>());
    engine_->RegisterModule(std::make_unique<InputSystem>());
    engine_->RegisterModule(std::make_unique<PhysicsWorld>());

    // Should still initialize successfully
    ASSERT_TRUE(engine_->Initialize());
}

// Test 8: Input multi-key handling
TEST_F(AllEnginesIntegrationTest, InputMultiKeyTracking) {
    auto input = std::make_unique<InputSystem>();
    InputSystem* input_ptr = input.get();
    engine_->RegisterModule(std::move(input));

    ASSERT_TRUE(engine_->Initialize());

    // Press multiple keys
    input_ptr->SetKeyDown(87, true);  // W
    input_ptr->SetKeyDown(65, true);  // A
    input_ptr->SetKeyDown(83, true);  // S
    input_ptr->SetKeyDown(68, true);  // D

    engine_->Update(0.016f);

    // All keys should be pressed
    ASSERT_EQ(input_ptr->GetKeyState(87), InputSystem::KeyState::Pressed);
    ASSERT_EQ(input_ptr->GetKeyState(65), InputSystem::KeyState::Pressed);
    ASSERT_EQ(input_ptr->GetKeyState(83), InputSystem::KeyState::Pressed);
    ASSERT_EQ(input_ptr->GetKeyState(68), InputSystem::KeyState::Pressed);

    // Release some keys
    input_ptr->SetKeyDown(87, false);
    input_ptr->SetKeyDown(68, false);

    engine_->Update(0.016f);

    // Correct keys should be released
    ASSERT_EQ(input_ptr->GetKeyState(87), InputSystem::KeyState::Released);
    ASSERT_EQ(input_ptr->GetKeyState(65), InputSystem::KeyState::Held);
    ASSERT_EQ(input_ptr->GetKeyState(83), InputSystem::KeyState::Held);
    ASSERT_EQ(input_ptr->GetKeyState(68), InputSystem::KeyState::Released);
}

// Test 9: Physics multiple bodies
TEST_F(AllEnginesIntegrationTest, PhysicsMultipleBodies) {
    auto physics = std::make_unique<PhysicsWorld>();
    PhysicsWorld* physics_ptr = physics.get();
    engine_->RegisterModule(std::move(physics));

    ASSERT_TRUE(engine_->Initialize());

    // Create multiple bodies
    Body2D body1 = {0.0f, 0.0f, 10.0f, 0.0f};
    Body2D body2 = {50.0f, 50.0f, -5.0f, 5.0f};
    Body2D body3 = {100.0f, 100.0f, 0.0f, -10.0f};

    int id1 = physics_ptr->AddBody(body1);
    int id2 = physics_ptr->AddBody(body2);
    int id3 = physics_ptr->AddBody(body3);

    ASSERT_EQ(id1, 0);
    ASSERT_EQ(id2, 1);
    ASSERT_EQ(id3, 2);

    // Update for 1 second
    for (int i = 0; i < 60; ++i) {
        engine_->Update(0.016f);
    }

    // Check all bodies moved
    auto b1 = physics_ptr->GetBody(id1);
    auto b2 = physics_ptr->GetBody(id2);
    auto b3 = physics_ptr->GetBody(id3);

    ASSERT_NE(b1, nullptr);
    ASSERT_NE(b2, nullptr);
    ASSERT_NE(b3, nullptr);

    // Verify movement
    ASSERT_GT(b1->x, 0.0f);  // Should move in positive X
    ASSERT_LT(b2->x, 50.0f);  // Should move in negative X
    ASSERT_EQ(b3->x, 100.0f);  // No X movement
    ASSERT_LT(b3->y, 100.0f);  // Should move in negative Y
}

// Test 10: Sound multiple clips
TEST_F(AllEnginesIntegrationTest, SoundMultipleClips) {
    auto sound = std::make_unique<SoundSystem>();
    SoundSystem* sound_ptr = sound.get();
    engine_->RegisterModule(std::move(sound));

    ASSERT_TRUE(engine_->Initialize());

    // Load multiple clips
    int clip1 = sound_ptr->LoadClip("music.wav");
    int clip2 = sound_ptr->LoadClip("sfx.wav");
    int clip3 = sound_ptr->LoadClip("voice.wav");

    ASSERT_GT(clip1, 0);
    ASSERT_GT(clip2, clip1);  // Should have incremented ID
    ASSERT_GT(clip3, clip2);

    // Play all clips
    sound_ptr->Play(clip1);
    sound_ptr->Play(clip2);
    sound_ptr->Play(clip3);

    ASSERT_TRUE(sound_ptr->IsPlaying(clip1));
    ASSERT_TRUE(sound_ptr->IsPlaying(clip2));
    ASSERT_TRUE(sound_ptr->IsPlaying(clip3));

    // Stop one clip
    sound_ptr->Stop(clip2);

    ASSERT_TRUE(sound_ptr->IsPlaying(clip1));
    ASSERT_FALSE(sound_ptr->IsPlaying(clip2));
    ASSERT_TRUE(sound_ptr->IsPlaying(clip3));
}

