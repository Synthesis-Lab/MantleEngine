#ifndef MANTLE_OPENGL_RENDERER_H
#define MANTLE_OPENGL_RENDERER_H

#include "renderer.h"
#include "render_packet.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <string>

// Forward declaration only - no include (GLFW3 may not be available)
struct GLFWwindow;

/// OpenGLRenderer - OpenGL Core 4.3+ rendering backend
/// Implements the Renderer interface using OpenGL Core profile
/// Uses GLFW3 for window/context management
class OpenGLRenderer final : public Renderer {
public:
    /// Constructor - initializes OpenGL renderer
    OpenGLRenderer();
    
    /// Destructor - cleans up OpenGL resources
    ~OpenGLRenderer() override;
    
    /// Initialize the renderer (GLFW window + GL context)
    bool Initialize() override;
    
    /// Shutdown the renderer and free all OpenGL resources
    void Shutdown() override;
    
    /// Check if renderer is ready to receive render packets
    bool IsReady() const override;
    
    /// Submit a render packet for processing
    void SubmitRenderPacket(const RenderPacket* packet) override;
    
    /// Wait for rendering to complete
    void WaitRender() override;
    
    /// Render a single frame from a RenderPacket
    void RenderFrame(const RenderPacket* packet) override;
    
    /// Upload vertex/index buffer data to GPU
    uint32_t UploadBuffer(const void* data, uint32_t size) override;
    
    /// Get the last error message
    const char* GetLastError() const override;
    
    /// Get renderer type
    Type GetType() const override { return Type::OPENGL; }
    
    /// Get renderer name
    const char* GetName() const override { return "OpenGL Core 4.3+"; }

private:
    // GLFW window management
    GLFWwindow* window_;
    int window_width_;
    int window_height_;
    
    // OpenGL state
    bool is_ready_;
    uint32_t frame_number_;
    
    // Frame rendering
    std::string error_message_;
    
    // Phase 5a: Framebuffer objects
    uint32_t framebuffer_object_;  // GL_FRAMEBUFFER
    uint32_t color_texture_;        // GL_COLOR_ATTACHMENT0
    uint32_t depth_renderbuffer_;   // GL_DEPTH_ATTACHMENT
    
    // Phase 5b: Graphics pipeline
    uint32_t shader_program_;       // Linked shader program
    uint32_t vertex_shader_;        // GL_VERTEX_SHADER
    uint32_t fragment_shader_;      // GL_FRAGMENT_SHADER
    
    // Phase 5c: Vertex/Index buffers
    uint32_t vertex_array_object_;  // VAO
    uint32_t quad_vertex_buffer_;   // VBO (quad geometry)
    uint32_t quad_index_buffer_;    // IBO (quad indices)
    uint32_t instance_buffer_;      // SSBO (instance data)
    
    // Phase 5h: Staging buffer for instance data (host-accessible)
    std::vector<uint8_t> staging_instance_buffer_;
    
    // Phase 5f: Texture management
    uint32_t texture_sampler_;      // GL_SAMPLER
    std::vector<uint32_t> textures_; // Texture IDs (GL_TEXTURE_2D)
    uint32_t placeholder_texture_;   // 1x1 white fallback texture
    
    // Phase 5d: Frame output and visualization
    std::vector<uint32_t> pixel_buffer_;  // CPU-side buffer for PPM dump
    bool enable_frame_dump_;
    uint32_t frame_dump_count_;
    
    // Helper methods
    bool CreateGLFWWindow();
    bool CreateGLContext();
    bool CreateShaderProgram();
    bool CreateQuadGeometry();
    bool CreateFramebufferObject();
    bool CreateTextureSampler();
    bool CreatePlaceholderTexture();
    
    // CPU-side software rendering for headless mode
    void RasterizeSprites(const RenderPacket* packet);
    
    void DumpCurrentFrame();
    bool SaveFrameToPPM(const char* filename, uint32_t width, uint32_t height);
    
    void SetError(const char* format, ...);
};

#endif // MANTLE_OPENGL_RENDERER_H
