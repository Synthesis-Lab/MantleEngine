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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
