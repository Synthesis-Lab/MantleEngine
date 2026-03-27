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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
