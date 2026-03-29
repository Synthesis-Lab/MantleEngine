#ifndef MANTLE_RENDERER_H
#define MANTLE_RENDERER_H

#include "render_packet.h"
#include <cstdint>
#include <memory>
#include <vector>

/// Abstract Renderer Interface - Base class for all graphics backends
/// Supports: Vulkan, OpenGL Core, DirectX 12 (future), Metal (future)
class Renderer {
public:
    /// Renderer types for backend selection
    enum class Type {
        VULKAN,      // Vulkan 1.2+
        OPENGL,      // OpenGL 4.3 core
        DIRECTX12,   // DirectX 12 (future)
        METAL,       // Metal 2.0+ (future)
        AUTO         // Auto-detect best available
    };
    
    /// Constructor - pure virtual
    Renderer() = default;
    
    /// Destructor - must be virtual for proper cleanup
    virtual ~Renderer() = default;
    
    /// Initialize the renderer
    /// Sets up GPU context, queues, memory pools
    /// Returns true on success
    virtual bool Initialize() = 0;
    
    /// Shutdown the renderer and free all GPU resources
    virtual void Shutdown() = 0;
    
    /// Check if renderer is ready to receive render packets
    virtual bool IsReady() const = 0;
    
    /// Submit a render packet for processing
    /// Queues the render packet for the next available frame
    virtual void SubmitRenderPacket(const RenderPacket* packet) = 0;
    
    /// Wait for rendering to complete
    /// Blocks until the current frame has been processed
    virtual void WaitRender() = 0;
    
    /// Render a single frame from a RenderPacket
    /// Called by WaitRender or automatically per frame
    virtual void RenderFrame(const RenderPacket* packet) = 0;
    
    /// Upload vertex/index buffer data to GPU
    /// Returns a texture/buffer ID for use in RenderPackets
    virtual uint32_t UploadBuffer(const void* data, uint32_t size) = 0;
    
    /// Get the last error message (for C ABI error reporting)
    virtual const char* GetLastError() const = 0;
    
    /// Get renderer type
    virtual Type GetType() const = 0;
    
    /// Get renderer name (for logging)
    virtual const char* GetName() const = 0;
    
    /// Factory function - creates appropriate renderer instance
    static std::unique_ptr<Renderer> Create(Type type = Type::AUTO);
};

#endif // MANTLE_RENDERER_H
