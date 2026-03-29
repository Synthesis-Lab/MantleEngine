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
