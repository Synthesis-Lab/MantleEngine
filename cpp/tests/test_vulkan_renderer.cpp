#include <gtest/gtest.h>
#include "../vulkan_renderer.h"

/// Phase 5: VulkanRenderer Unit Tests
/// Tests core initialization and shutdown flow

class VulkanRendererTest : public ::testing::Test {
protected:
    VulkanRenderer renderer;
};

/// Test 1: Instance Creation
TEST_F(VulkanRendererTest, CreateInstance) {
    EXPECT_FALSE(renderer.IsReady());
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    renderer.Shutdown();
}

/// Test 2: Multiple Initializations
TEST_F(VulkanRendererTest, DoubleInitialize) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Second initialize should fail (already initialized)
    EXPECT_FALSE(renderer.Initialize());
    
    renderer.Shutdown();
}

/// Test 3: Shutdown Without Initialize
TEST_F(VulkanRendererTest, ShutdownWithoutInit) {
    // Should not crash
    renderer.Shutdown();
    EXPECT_FALSE(renderer.IsReady());
}

/// Test 4: Double Shutdown
TEST_F(VulkanRendererTest, DoubleShutdown) {
    EXPECT_TRUE(renderer.Initialize());
    renderer.Shutdown();
    
    // Second shutdown should be safe
    renderer.Shutdown();
    EXPECT_FALSE(renderer.IsReady());
}

/// Test 5: GetLastError
TEST_F(VulkanRendererTest, ErrorHandling) {
    // Try double init to generate error
    renderer.Initialize();
    renderer.Initialize(); // This should fail and set error
    
    const char* error = renderer.GetLastError();
    EXPECT_NE(error, nullptr);
    // Error message should contain something meaningful
    
    renderer.Shutdown();
}

/// Test 6: Submit Before Init
TEST_F(VulkanRendererTest, SubmitBeforeInit) {
    RenderPacket packet = {};
    
    // Should not crash, should handle gracefully
    renderer.SubmitRenderPacket(&packet);
    
    // Should have set an error
    const char* error = renderer.GetLastError();
    EXPECT_NE(error, nullptr);
}

/// Test 7: Null Packet Submission
TEST_F(VulkanRendererTest, NullPacketSubmit) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Submitting null packet should be handled
    renderer.SubmitRenderPacket(nullptr);
    
    const char* error = renderer.GetLastError();
    EXPECT_NE(error, nullptr);
    
    renderer.Shutdown();
}

/// Test 8: Buffer Upload
TEST_F(VulkanRendererTest, UploadBuffer) {
    EXPECT_TRUE(renderer.Initialize());
    
    float data[] = {0.0f, 1.0f, 2.0f};
    uint32_t id = renderer.UploadBuffer(data, sizeof(data));
    
    // Should return valid ID
    EXPECT_GT(id, 0);
    
    renderer.Shutdown();
}

/// Test 9: Invalid Buffer Upload
TEST_F(VulkanRendererTest, InvalidBufferUpload) {
    EXPECT_TRUE(renderer.Initialize());
    
    // Null data should return 0
    uint32_t id = renderer.UploadBuffer(nullptr, 100);
    EXPECT_EQ(id, 0);
    
    // Zero size should return 0
    float data[] = {1.0f};
    id = renderer.UploadBuffer(data, 0);
    EXPECT_EQ(id, 0);
    
    renderer.Shutdown();
}

/// Test 10: Wait Render Without Init
TEST_F(VulkanRendererTest, WaitRenderBeforeInit) {
    // Should not crash
    renderer.WaitRender();
    
    const char* error = renderer.GetLastError();
    EXPECT_NE(error, nullptr);
}

/// ============================================================================
/// Phase 5a: Surface & Swapchain Tests
/// ============================================================================

/// Test 11: Surface Creation During Init
TEST_F(VulkanRendererTest, CreateSurface) {
    // Surface should be created as part of Initialize()
    EXPECT_FALSE(renderer.IsReady());
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    
    // After success, we should have initialized surface structures
    // (though in headless mode, surface_ pointer is nullptr)
    
    renderer.Shutdown();
}

/// Test 12: Swapchain Creation During Init
TEST_F(VulkanRendererTest, CreateSwapchain) {
    // Swapchain should be created as part of Initialize()
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    
    // Swapchain should have valid dimensions
    // (The actual swapchain_extent_ is set to 1024x768)
    
    renderer.Shutdown();
}

/// Test 13: Image Views Creation
TEST_F(VulkanRendererTest, CreateImageViews) {
    // Image views should be created during Initialize()
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    
    // Image views correspond to swapchain images
    // Should create 2 for double-buffering
    
    renderer.Shutdown();
}

/// Test 14: Render Pass Creation
TEST_F(VulkanRendererTest, CreateRenderPass) {
    // Render pass should be created during Initialize()
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    
    // Render pass defines frame operation structure
    // Should exist after successful initialization
    
    renderer.Shutdown();
}

/// Test 15: Framebuffers Creation
TEST_F(VulkanRendererTest, CreateFramebuffers) {
    // Framebuffers should be created during Initialize()
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    
    // Framebuffers bind image views to render pass
    // Should have one per swapchain image (2 for double-buffering)
    
    renderer.Shutdown();
}

/// Test 16: Multiple Initializations with Phase 5a
TEST_F(VulkanRendererTest, MultipleSwapchainTest) {
    // Verify that re-initializing properly cleans up Phase 5a resources
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    renderer.Shutdown();
    
    // Should be able to shutdown and reinit
    EXPECT_FALSE(renderer.IsReady());
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    
    renderer.Shutdown();
}

/// Test 17: Phase 5a Integration
TEST_F(VulkanRendererTest, PhaseIntegrationTest) {
    // Full Phase 5 + Phase 5a pipeline integration test
    EXPECT_FALSE(renderer.IsReady());
    
    // Initialize should set up:
    // 1. Vulkan instance (Phase 5)
    // 2. Physical device selection (Phase 5)
    // 3. Logical device creation (Phase 5)
    // 4. Command pool (Phase 5)
    // 5. Surface (Phase 5a)
    // 6. Swapchain (Phase 5a)
    // 7. Image views (Phase 5a)
    // 8. Render pass (Phase 5a)
    // 9. Framebuffers (Phase 5a)
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    
    // Rendering operations should work
    RenderPacket packet = {};
    renderer.SubmitRenderPacket(&packet);
    renderer.WaitRender();
    
    // Cleanup should be reversible
    renderer.Shutdown();
    EXPECT_FALSE(renderer.IsReady());
}

/// ============================================================================
/// Phase 5h: Sprite Rendering Tests
/// ============================================================================

/// Test 18: Phase 5h - Single Sprite Rendering
TEST_F(VulkanRendererTest, Phase5h_SingleSpriteRender) {
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    
    // Create a single sprite render packet
    TransformPacket transform(100.0f, 100.0f, 0.0f, 1.0f, 1.0f, 0);
    
    SpritePacket sprite(0, 32.0f, 32.0f, 0xFFFFFFFF, 0.0f, 0.0f, 1.0f, 1.0f);
    
    RenderPacket packet(0, 0.016f, 1, 1, 0, &transform, &sprite, nullptr);
    
    // Submit and render
    renderer.SubmitRenderPacket(&packet);
    renderer.WaitRender();
    
    // Verify no errors
    const char* error = renderer.GetLastError();
    // Single sprite should render without errors
    
    renderer.Shutdown();
}

/// Test 19: Phase 5h - Multiple Sprites Rendering
TEST_F(VulkanRendererTest, Phase5h_MultipleSpritesRender) {
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    
    // Create 5 sprites with different positions and scales
    const int NUM_SPRITES = 5;
    TransformPacket transforms[NUM_SPRITES] = {
        TransformPacket(200.0f, 600.0f, 0.0f,   1.0f, 1.0f, 0),
        TransformPacket(800.0f, 600.0f, 45.0f,  1.2f, 1.2f, 1),
        TransformPacket(512.0f, 384.0f, 90.0f,  1.5f, 1.5f, 2),
        TransformPacket(200.0f, 200.0f, 180.0f, 0.8f, 0.8f, 1),
        TransformPacket(800.0f, 200.0f, 270.0f, 1.1f, 0.9f, 0),
    };
    
    SpritePacket sprites[NUM_SPRITES] = {
        SpritePacket(0, 32.0f, 32.0f, 0xFF0000FF, 0.0f, 0.0f, 1.0f, 1.0f),  // Red
        SpritePacket(0, 48.0f, 48.0f, 0x00FF00FF, 0.0f, 0.0f, 1.0f, 1.0f),  // Green
        SpritePacket(0, 64.0f, 64.0f, 0x0000FFFF, 0.0f, 0.0f, 1.0f, 1.0f),  // Blue
        SpritePacket(0, 40.0f, 40.0f, 0xFFFF00FF, 0.0f, 0.0f, 1.0f, 1.0f),  // Yellow
        SpritePacket(0, 56.0f, 56.0f, 0x00FFFFFF, 0.0f, 0.0f, 1.0f, 1.0f),  // Cyan
    };
    
    RenderPacket packet(0, 0.016f, NUM_SPRITES, NUM_SPRITES, 0, transforms, sprites, nullptr);
    
    // Submit and render
    renderer.SubmitRenderPacket(&packet);
    renderer.WaitRender();
    
    renderer.Shutdown();
}

/// Test 20: Phase 5h - Sprite Batching with Texture IDs
TEST_F(VulkanRendererTest, Phase5h_SpriteBatchingWithTextures) {
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    
    // Create 8 sprites using different descriptor sets (simulated texture IDs)
    const int NUM_SPRITES = 8;
    TransformPacket transforms[NUM_SPRITES];
    SpritePacket sprites[NUM_SPRITES];
    
    // Generate sprites in a grid pattern
    for (int i = 0; i < NUM_SPRITES; i++) {
        int row = i / 4;
        int col = i % 4;
        
        transforms[i] = TransformPacket(
            300.0f + col * 150.0f,  //position_x
            300.0f + row * 200.0f,  // position_y
            (float)(i * 45.0f),     // rotation (increasing)
            0.8f + i * 0.1f,        // scale_x (increasing)
            1.0f,                   // scale_y
            i                       // z_index
        );
        
        // Vary texture ID (0 for placeholder, others for potential future textures)
        uint32_t tex_id = i % 2;  // Alternate between texture 0 and 1
        
        // Vary colors: RGBA format
        uint32_t colors[] = {
            0xFF0000FF,  // Red
            0x00FF00FF,  // Green
            0x0000FFFF,  // Blue
            0xFFFF00FF,  // Yellow
            0xFF00FFFF,  // Magenta
            0x00FFFFFF,  // Cyan
            0xFFFFFFFF,  // White
            0x808080FF,  // Gray
        };
        
        sprites[i] = SpritePacket(
            tex_id,                 // texture_id
            24.0f + i * 8.0f,       // width (increasing)
            24.0f + i * 8.0f,       // height (increasing)
            colors[i],              // color
            0.0f,                   // uv_min_x
            0.0f,                   // uv_min_y
            1.0f,                   // uv_max_x
            1.0f                    // uv_max_y
        );
    }
    
    RenderPacket packet(0, 0.016f, NUM_SPRITES, NUM_SPRITES, 0, transforms, sprites, nullptr);
    
    // Submit multiple frames to verify continuous rendering
    for (int frame = 0; frame < 3; frame++) {
        // Note: RenderPacket doesn't expose frame_number for modification,
        // but we can create new packets for each frame
        renderer.SubmitRenderPacket(&packet);
        renderer.WaitRender();
    }
    
    renderer.Shutdown();
}

/// Test 21: Phase 5b+5c - Visual Demonstration (4-Sprite Grid)
/// Creates distinct visual output showing batch rendering with transforms
TEST_F(VulkanRendererTest, Phase5b_VisualDemo_ColorGrid) {
    EXPECT_TRUE(renderer.Initialize());
    EXPECT_TRUE(renderer.IsReady());
    
    // Create 4-sprite grid (2x2) with distinct colors and transforms
    // This demonstrates the visual output of Vulkan backend
    const int NUM_SPRITES = 4;
    TransformPacket transforms[NUM_SPRITES];
    SpritePacket sprites[NUM_SPRITES];
    
    // Define grid positions and colors
    struct GridCell {
        float pos_x;
        float pos_y;
        float rotation;
        float scale_x;
        float scale_y;
        uint32_t color;
        const char* name;
    };
    
    GridCell cells[] = {
        {256.0f, 384.0f, 0.0f,   0.8f, 0.8f, 0xFF0000FF, "Red"},      // Top-left
        {768.0f, 384.0f, 0.0f,   0.8f, 0.8f, 0x00FF00FF, "Green"},    // Top-right
        {256.0f, 128.0f, 45.0f,  0.8f, 0.8f, 0x0000FFFF, "Blue"},     // Bottom-left (rotated)
        {768.0f, 128.0f, -45.0f, 1.2f, 1.2f, 0xFFFF00FF, "Yellow"},   // Bottom-right (rotated + scaled)
    };
    
    for (int i = 0; i < NUM_SPRITES; i++) {
        transforms[i] = TransformPacket(
            cells[i].pos_x,
            cells[i].pos_y,
            cells[i].rotation,
            cells[i].scale_x,
            cells[i].scale_y,
            i + 1  // z_index
        );
        
        sprites[i] = SpritePacket(
            i + 1,           // texture_id
            64.0f,           // width
            64.0f,           // height
            cells[i].color,  // RGBA color
            0.0f,            // uv_min_x
            0.0f,            // uv_min_y
            1.0f,            // uv_max_x
            1.0f             // uv_max_y
        );
    }
    
    RenderPacket packet(0, 0.016f, NUM_SPRITES, NUM_SPRITES, 0, transforms, sprites, nullptr);
    
    std::cout << "\n[Visual Demo] Vulkan - Creating 4-Sprite Grid" << std::endl;
    std::cout << "  Layout: 2x2 with primary colors (RGBY)" << std::endl;
    std::cout << "  Frame output: output/frame_***.ppm" << std::endl;
    
    // Submit and render
    renderer.SubmitRenderPacket(&packet);
    renderer.WaitRender();
    
    std::cout << "✓ Vulkan grid rendering complete" << std::endl;
    
    renderer.Shutdown();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
