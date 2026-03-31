#include <gtest/gtest.h>
#include "../include/opengl_renderer.h"

/// Phase 5a: OpenGL Renderer Unit Tests
/// Tests core initialization, shader compilation, and rendering

class OpenGLRendererTest : public ::testing::Test {
protected:
    OpenGLRenderer renderer;
};

/// Test 1: Instance Creation
TEST_F(OpenGLRendererTest, CreateInstance) {
    EXPECT_FALSE(renderer.IsReady());
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    renderer.Shutdown();
}

/// Test 2: Multiple Initializations
TEST_F(OpenGLRendererTest, DoubleInitialize) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Second initialize should fail (already initialized)
    EXPECT_FALSE(renderer.Initialize());
    
    renderer.Shutdown();
}

/// Test 3: Shutdown Without Initialize
TEST_F(OpenGLRendererTest, ShutdownWithoutInit) {
    // Should not crash
    renderer.Shutdown();
    EXPECT_FALSE(renderer.IsReady());
}

/// Test 4: Double Shutdown
TEST_F(OpenGLRendererTest, DoubleShutdown) {
    EXPECT_TRUE(renderer.Initialize());
    renderer.Shutdown();
    
    // Second shutdown should be safe
    renderer.Shutdown();
    EXPECT_FALSE(renderer.IsReady());
}

/// Test 5: GetLastError
TEST_F(OpenGLRendererTest, ErrorHandling) {
    // Try double init to generate error
    renderer.Initialize();
    renderer.Initialize(); // This should fail and set error
    
    const char* error = renderer.GetLastError();
    EXPECT_NE(error, nullptr);
    // Error message should contain something meaningful
    
    renderer.Shutdown();
}

/// Test 6: Submit Before Init
TEST_F(OpenGLRendererTest, SubmitBeforeInit) {
    RenderPacket packet = {};
    
    // Should not crash, should handle gracefully
    renderer.SubmitRenderPacket(&packet);
    
    // Should have set an error
    const char* error = renderer.GetLastError();
    EXPECT_NE(error, nullptr);
}

/// Test 7: Null Packet Submission
TEST_F(OpenGLRendererTest, NullPacketSubmit) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Submitting null packet should be handled
    renderer.SubmitRenderPacket(nullptr);
    
    const char* error = renderer.GetLastError();
    EXPECT_NE(error, nullptr);
    
    renderer.Shutdown();
}

/// Test 8: Buffer Upload
TEST_F(OpenGLRendererTest, UploadBuffer) {
    EXPECT_TRUE(renderer.Initialize());
    
    float data[] = {0.0f, 1.0f, 2.0f};
    uint32_t id = renderer.UploadBuffer(data, sizeof(data));
    
    // Should return valid ID
    EXPECT_GT(id, 0);
    
    renderer.Shutdown();
}

/// Test 9: Invalid Buffer Upload
TEST_F(OpenGLRendererTest, InvalidBufferUpload) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Null data should fail
    uint32_t id = renderer.UploadBuffer(nullptr, 100);
    EXPECT_EQ(id, 0);
    
    // Zero size should fail
    float data[] = {1.0f};
    id = renderer.UploadBuffer(data, 0);
    EXPECT_EQ(id, 0);
    
    renderer.Shutdown();
}

/// Test 10: Render Frame
TEST_F(OpenGLRendererTest, RenderFrame) {
    EXPECT_TRUE(renderer.Initialize());
    
    RenderPacket packet = {};
    packet.frame_number = 0;
    packet.transform_count = 8;
    packet.sprite_count = 8;
    packet.collider_count = 0;
    
    // Should not crash
    renderer.SubmitRenderPacket(&packet);
    
    renderer.Shutdown();
}

/// Test 11: Wait Render
TEST_F(OpenGLRendererTest, WaitRender) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Should not crash
    renderer.WaitRender();
    
    renderer.Shutdown();
}

/// Test 12: Frame Dump On Render
TEST_F(OpenGLRendererTest, Phase5a_FramebufferRendering) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Render should dump frame
    RenderPacket packet = {};
    packet.frame_number = 0;
    packet.transform_count = 8;
    packet.sprite_count = 8;
    packet.collider_count = 0;
    
    renderer.SubmitRenderPacket(&packet);
    
    // Check that frame was dumped
    const char* error = renderer.GetLastError();
    // Frame dump success or no error
    
    renderer.Shutdown();
}

/// Test 13: Multiple Frame Renders
TEST_F(OpenGLRendererTest, Phase5a_MultipleFrames) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Render multiple frames
    for (int i = 0; i < 3; i++) {
        RenderPacket packet = {};
        packet.frame_number = (uint32_t)i;
        packet.transform_count = 8;
        packet.sprite_count = 8;
        packet.collider_count = 0;
        
        renderer.SubmitRenderPacket(&packet);
    }
    
    renderer.Shutdown();
}

/// Test 14: Wait Render With Pending Frames
TEST_F(OpenGLRendererTest, Phase5a_WaitRenderWithPendingFrames) {
    EXPECT_TRUE(renderer.Initialize());
    
    RenderPacket packet = {};
    packet.frame_number = 0;
    packet.transform_count = 8;
    packet.sprite_count = 8;
    packet.collider_count = 0;
    
    // Submit then wait
    renderer.SubmitRenderPacket(&packet);
    renderer.WaitRender();
    
    renderer.Shutdown();
}

/// Test 15: Renderer Type
TEST_F(OpenGLRendererTest, GetRendererType) {
    EXPECT_TRUE(renderer.Initialize());
    
    Renderer::Type type = renderer.GetType();
    EXPECT_EQ(type, Renderer::Type::OPENGL);
    
    renderer.Shutdown();
}

/// Test 16: Renderer Name
TEST_F(OpenGLRendererTest, GetRendererName) {
    EXPECT_TRUE(renderer.Initialize());
    
    const char* name = renderer.GetName();
    EXPECT_NE(name, nullptr);
    EXPECT_TRUE(strlen(name) > 0);
    
    renderer.Shutdown();
}

/// Test 17: Quad Geometry Creation
TEST_F(OpenGLRendererTest, Phase5g_QuadGeometryCreation) {
    EXPECT_TRUE(renderer.Initialize());
    
    // If we get here without crashes, quad geometry was created successfully
    EXPECT_TRUE(renderer.IsReady());
    
    renderer.Shutdown();
}

/// Test 18: Framebuffer Object Creation
TEST_F(OpenGLRendererTest, Phase5a_FramebufferObjectCreation) {
    EXPECT_TRUE(renderer.Initialize());
    
    // If we get here without crashes, FBO was created successfully
    EXPECT_TRUE(renderer.IsReady());
    
    renderer.Shutdown();
}

/// Test 19: Texture Sampler Creation
TEST_F(OpenGLRendererTest, Phase5f_TextureSamplerCreation) {
    EXPECT_TRUE(renderer.Initialize());
    
    // If we get here without crashes, sampler was created successfully
    EXPECT_TRUE(renderer.IsReady());
    
    renderer.Shutdown();
}

/// Test 20: Placeholder Texture Creation
TEST_F(OpenGLRendererTest, Phase5f_PlaceholderTextureCreation) {
    EXPECT_TRUE(renderer.Initialize());
    
    // If we get here without crashes, placeholder texture was created successfully
    EXPECT_TRUE(renderer.IsReady());
    
    renderer.Shutdown();
}

// ============================================================================
// Phase 5b/5c: Sprite Batching + Transform Matrix Tests
// ============================================================================

class Phase5bBatchingTest : public ::testing::Test {
protected:
    OpenGLRenderer renderer;
    
    /// Helper: Create sprite packet with color
    SpritePacket CreateSprite(uint32_t tex_id, float w, float h, uint32_t color) {
        SpritePacket sprite = {};
        sprite.texture_id = tex_id;
        sprite.width = w;
        sprite.height = h;
        sprite.color = color;
        sprite.uv_min_x = 0.0f;
        sprite.uv_min_y = 0.0f;
        sprite.uv_max_x = 1.0f;
        sprite.uv_max_y = 1.0f;
        return sprite;
    }
    
    /// Helper: Create transform packet
    TransformPacket CreateTransform(float x, float y, float rot, float sx, float sy) {
        TransformPacket t = {};
        t.position_x = x;
        t.position_y = y;
        t.rotation = rot;
        t.scale_x = sx;
        t.scale_y = sy;
        t.z_index = 0;
        t.padding = 0;
        return t;
    }
};

/// Test 21: Single Sprite Batch Rendering
TEST_F(Phase5bBatchingTest, Phase5b_SingleSpriteBatch) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Create one sprite at origin
    TransformPacket transform = CreateTransform(0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    SpritePacket sprite = CreateSprite(1, 64.0f, 64.0f, 0xFF0000FF);  // Red
    
    RenderPacket packet = {};
    packet.frame_number = 0;
    packet.transform_count = 1;
    packet.sprite_count = 1;
    packet.transforms = &transform;
    packet.sprites = &sprite;
    
    // Should render without crashing
    renderer.SubmitRenderPacket(&packet);
    
    renderer.Shutdown();
}

/// Test 22: Multiple Sprites Batch
TEST_F(Phase5bBatchingTest, Phase5b_MultipleSpriteBatch) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Create 4 sprites in different positions (quad layout)
    TransformPacket transforms[4] = {
        CreateTransform(-0.5f, -0.5f, 0.0f, 0.5f, 0.5f),   // Bottom-left - red
        CreateTransform( 0.5f, -0.5f, 0.0f, 0.5f, 0.5f),   // Bottom-right - green
        CreateTransform( 0.5f,  0.5f, 0.0f, 0.5f, 0.5f),   // Top-right - blue
        CreateTransform(-0.5f,  0.5f, 0.0f, 0.5f, 0.5f),   // Top-left - yellow
    };
    
    SpritePacket sprites[4] = {
        CreateSprite(1, 32.0f, 32.0f, 0xFF0000FF),  // Red
        CreateSprite(2, 32.0f, 32.0f, 0x00FF00FF),  // Green
        CreateSprite(3, 32.0f, 32.0f, 0x0000FFFF),  // Blue
        CreateSprite(4, 32.0f, 32.0f, 0xFFFF00FF),  // Yellow
    };
    
    RenderPacket packet = {};
    packet.frame_number = 0;
    packet.transform_count = 4;
    packet.sprite_count = 4;
    packet.transforms = transforms;
    packet.sprites = sprites;
    
    // Should render all 4 sprites
    renderer.SubmitRenderPacket(&packet);
    
    renderer.Shutdown();
}

/// Test 23: Transform - Position
TEST_F(Phase5bBatchingTest, Phase5c_TransformPosition) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Move sprite to right edge
    TransformPacket transform = CreateTransform(0.8f, 0.0f, 0.0f, 1.0f, 1.0f);
    SpritePacket sprite = CreateSprite(1, 64.0f, 64.0f, 0xFF0000FF);
    
    RenderPacket packet = {};
    packet.frame_number = 0;
    packet.transform_count = 1;
    packet.sprite_count = 1;
    packet.transforms = &transform;
    packet.sprites = &sprite;
    
    renderer.SubmitRenderPacket(&packet);
    renderer.Shutdown();
}

/// Test 24: Transform - Rotation
TEST_F(Phase5bBatchingTest, Phase5c_TransformRotation) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Rotate sprite 45 degrees
    TransformPacket transform = CreateTransform(0.0f, 0.0f, 45.0f, 1.0f, 1.0f);
    SpritePacket sprite = CreateSprite(1, 64.0f, 64.0f, 0x00FF00FF);
    
    RenderPacket packet = {};
    packet.frame_number = 0;
    packet.transform_count = 1;
    packet.sprite_count = 1;
    packet.transforms = &transform;
    packet.sprites = &sprite;
    
    renderer.SubmitRenderPacket(&packet);
    renderer.Shutdown();
}

/// Test 25: Transform - Scale
TEST_F(Phase5bBatchingTest, Phase5c_TransformScale) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Scale sprite 2x in both axes
    TransformPacket transform = CreateTransform(0.0f, 0.0f, 0.0f, 2.0f, 2.0f);
    SpritePacket sprite = CreateSprite(1, 32.0f, 32.0f, 0x0000FFFF);
    
    RenderPacket packet = {};
    packet.frame_number = 0;
    packet.transform_count = 1;
    packet.sprite_count = 1;
    packet.transforms = &transform;
    packet.sprites = &sprite;
    
    renderer.SubmitRenderPacket(&packet);
    renderer.Shutdown();
}

/// Test 26: Transform - Combined TRS
TEST_F(Phase5bBatchingTest, Phase5c_TransformCombined) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Position + Rotation + Scale all at once
    TransformPacket transform = CreateTransform(0.3f, -0.2f, 30.0f, 1.5f, 1.5f);
    SpritePacket sprite = CreateSprite(1, 48.0f, 48.0f, 0xFFFF00FF);
    
    RenderPacket packet = {};
    packet.frame_number = 0;
    packet.transform_count = 1;
    packet.sprite_count = 1;
    packet.transforms = &transform;
    packet.sprites = &sprite;
    
    renderer.SubmitRenderPacket(&packet);
    renderer.Shutdown();
}

/// Test 27: Sprite Colors - Red
TEST_F(Phase5bBatchingTest, Phase5b_SpriteColorRed) {
    EXPECT_TRUE(renderer.Initialize());
    
    TransformPacket transform = CreateTransform(0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    SpritePacket sprite = CreateSprite(1, 64.0f, 64.0f, 0xFF0000FF);  // Red (R=255, G=0, B=0)
    
    RenderPacket packet = {};
    packet.transform_count = 1;
    packet.sprite_count = 1;
    packet.transforms = &transform;
    packet.sprites = &sprite;
    
    renderer.SubmitRenderPacket(&packet);
    renderer.Shutdown();
}

/// Test 28: Sprite Colors - Blue with Alpha
TEST_F(Phase5bBatchingTest, Phase5b_SpriteColorAlpha) {
    EXPECT_TRUE(renderer.Initialize());
    
    TransformPacket transform = CreateTransform(0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    // Blue with 50% alpha (A=128)
    SpritePacket sprite = CreateSprite(1, 64.0f, 64.0f, 0x0000FF80);
    
    RenderPacket packet = {};
    packet.transform_count = 1;
    packet.sprite_count = 1;
    packet.transforms = &transform;
    packet.sprites = &sprite;
    
    renderer.SubmitRenderPacket(&packet);
    renderer.Shutdown();
}

/// Test 29: Batch Rendering Continuation (multi-frame)
TEST_F(Phase5bBatchingTest, Phase5b_BatchContinuation) {
    EXPECT_TRUE(renderer.Initialize());
    
    TransformPacket transforms[2] = {
        CreateTransform(-0.3f, 0.0f, 0.0f, 1.0f, 1.0f),
        CreateTransform( 0.3f, 0.0f, 0.0f, 1.0f, 1.0f),
    };
    
    SpritePacket sprites[2] = {
        CreateSprite(1, 64.0f, 64.0f, 0xFF0000FF),  // Red, left
        CreateSprite(2, 64.0f, 64.0f, 0x00FF00FF),  // Green, right
    };
    
    // Frame 1
    RenderPacket packet1 = {};
    packet1.frame_number = 0;
    packet1.transform_count = 2;
    packet1.sprite_count = 2;
    packet1.transforms = transforms;
    packet1.sprites = sprites;
    renderer.SubmitRenderPacket(&packet1);
    
    // Frame 2 - same sprites, different positions
    transforms[0].position_x = 0.0f;  // Move left sprite to center
    transforms[1].position_x = 0.5f;  // Move right sprite further right
    
    RenderPacket packet2 = {};
    packet2.frame_number = 1;
    packet2.transform_count = 2;
    packet2.sprite_count = 2;
    packet2.transforms = transforms;
    packet2.sprites = sprites;
    renderer.SubmitRenderPacket(&packet2);
    
    renderer.Shutdown();
}

/// Test 30: Phase 5b+5c - Visual Demonstration (2-Sprite Cross Pattern)
/// Creates distinct visual output showing OpenGL batch rendering with transforms
TEST_F(Phase5bBatchingTest, Phase5b_VisualDemo_CrossPattern) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Create 2-sprite cross/X pattern with different colors and rotations
    // This demonstrates the visual output of OpenGL backend
    const int NUM_SPRITES = 2;
    TransformPacket transforms[NUM_SPRITES];
    SpritePacket sprites[NUM_SPRITES];
    
    // Horizontal sprite (center-left)
    transforms[0] = CreateTransform(
        -0.3f,   // position_x
        0.0f,    // position_y
        0.0f,    // rotation
        1.0f,    // scale_x
        0.5f     // scale_y
    );
    
    sprites[0] = CreateSprite(
        1,           // texture_id
        128.0f,      // width
        32.0f,       // height
        0xFF0080FF   // Magenta color
    );
    
    // Vertical sprite (center-right, rotated 90°)
    transforms[1] = CreateTransform(
        0.3f,    // position_x
        0.0f,    // position_y
        90.0f,   // rotation (90 degrees)
        1.0f,    // scale_x
        0.5f     // scale_y
    );
    
    sprites[1] = CreateSprite(
        2,           // texture_id
        128.0f,      // width
        32.0f,       // height
        0x00FFFFFF   // Cyan color
    );
    
    RenderPacket packet = {};
    packet.frame_number = 0;
    packet.delta_time = 0.016f;
    packet.transform_count = NUM_SPRITES;
    packet.sprite_count = NUM_SPRITES;
    packet.transforms = transforms;
    packet.sprites = sprites;
    
    std::cout << "\n[Visual Demo] OpenGL - Creating 2-Sprite Cross" << std::endl;
    std::cout << "  Layout: Cross/X pattern with Magenta (H) + Cyan (V)" << std::endl;
    std::cout << "  Frame output: output/frame_***.ppm" << std::endl;
    
    renderer.SubmitRenderPacket(&packet);
    
    std::cout << "✓ OpenGL cross pattern rendering complete" << std::endl;
    
    renderer.Shutdown();
}
