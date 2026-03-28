#ifndef MANTLE_VULKAN_RENDERER_H
#define MANTLE_VULKAN_RENDERER_H

#include "render_packet.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

/// Forward declarations for Vulkan types
struct VkInstance_T;
struct VkPhysicalDevice_T;
struct VkDevice_T;
struct VkQueue_T;
struct VkCommandPool_T;
struct VkCommandBuffer_T;
struct VkSurfaceKHR_T;
struct VkSwapchainKHR_T;
struct VkImage_T;
struct VkImageView_T;
struct VkRenderPass_T;
struct VkFramebuffer_T;
struct VkSemaphore_T;
struct VkFence_T;
struct VkPipeline_T;

typedef VkInstance_T* VkInstance;
typedef VkPhysicalDevice_T* VkPhysicalDevice;
typedef VkDevice_T* VkDevice;
typedef VkQueue_T* VkQueue;
typedef VkCommandPool_T* VkCommandPool;
typedef VkCommandBuffer_T* VkCommandBuffer;
typedef VkSurfaceKHR_T* VkSurfaceKHR;
typedef VkSwapchainKHR_T* VkSwapchainKHR;
typedef VkRenderPass_T* VkRenderPass;
typedef VkFramebuffer_T* VkFramebuffer;
typedef VkSemaphore_T* VkSemaphore;
typedef VkFence_T* VkFence;
typedef VkPipeline_T* VkPipeline;

/// VulkanRenderer - Core 2D rendering engine
/// Manages Vulkan initialization, command buffers, and rendering pipeline
class VulkanRenderer {
public:
    /// Constructor - initializes Vulkan instance
    VulkanRenderer();
    
    /// Destructor - cleans up Vulkan resources
    ~VulkanRenderer();
    
    /// Initialize the renderer (called from mantle_renderer_init)
    /// Sets up instance, device, queues, command pools
    /// Returns true on success
    bool Initialize();
    
    /// Shutdown the renderer and free all Vulkan resources
    void Shutdown();
    
    /// Check if renderer is ready to receive render packets
    bool IsReady() const;
    
    /// Submit a render packet for processing
    /// Queues the render packet for the next available frame
    void SubmitRenderPacket(const RenderPacket* packet);
    
    /// Wait for rendering to complete
    /// Blocks until the current frame has been processed
    void WaitRender();
    
    /// Upload vertex/index buffer data to GPU
    /// Returns a texture/buffer ID for use in RenderPackets
    uint32_t UploadBuffer(const void* data, uint32_t size);
    
    /// Get the last error message (for C ABI error reporting)
    const char* GetLastError() const;
    
    /// Render a single frame from a RenderPacket
    /// Called by WaitRender or automatically per frame
    void RenderFrame(const RenderPacket* packet);

private:
    // Vulkan handle members
    VkInstance instance_;
    VkPhysicalDevice physical_device_;
    VkDevice device_;
    VkQueue graphics_queue_;
    VkQueue present_queue_;
    VkCommandPool command_pool_;
    
    // Phase 5a: Surface and swapchain members
    VkSurfaceKHR surface_;
    VkSwapchainKHR swapchain_;
    std::vector<VkImage> swapchain_images_;
    std::vector<VkImageView> image_views_;
    VkFormat swapchain_image_format_;
    VkExtent2D swapchain_extent_;
    
    // Phase 5a: Render pass and framebuffers
    VkRenderPass render_pass_;
    std::vector<VkFramebuffer> framebuffers_;
    
    // Phase 5b: Graphics pipeline
    VkPipelineLayout pipeline_layout_;
    VkPipeline graphics_pipeline_;
    
    // Phase 5c: Command buffers & Frame rendering
    std::vector<VkCommandBuffer> command_buffers_;
    VkFence render_fence_;
    
    // Queue family indices
    uint32_t graphics_family_;
    uint32_t present_family_;
    
    // Rendering state
    bool is_initialized_;
    bool is_ready_;
    uint32_t frame_number_;
    
    // Error handling
    mutable char last_error_[256];
    
    // Helper methods for initialization
    bool CreateInstance();
    bool SelectPhysicalDevice();
    bool CreateLogicalDevice();
    bool CreateCommandPool();
    
    // Phase 5a: Surface & Swapchain
    bool CreateSurface();
    bool CreateSwapchain();
    bool CreateImageViews();
    bool CreateRenderPass();
    bool CreateFramebuffers();
    
    // Phase 5b: Render Pass & Pipelines (enhanced)
    bool CreatePipelineLayout();
    bool CreateGraphicsPipeline();
    VkShaderModule CreateShaderModule(const void* code, uint32_t size);
    void DestroyShaderModule(VkShaderModule module);
    
    // Phase 5c: Command buffer recording & frame submission
    bool AllocateCommandBuffers();
    bool CreateFence();
    void DestroyFence();
    bool RecordRenderCommands(VkCommandBuffer cmd_buffer, uint32_t image_index);
    bool SubmitFrame(uint32_t image_index);
    
    // Helper methods for rendering
    void RecordCommandBuffer(uint32_t image_index, const RenderPacket* packet);
    void RenderTransforms(VkCommandBuffer cmd_buffer, const RenderPacket* packet);
    void RenderSprites(VkCommandBuffer cmd_buffer, const RenderPacket* packet);
    void RenderColliders(VkCommandBuffer cmd_buffer, const RenderPacket* packet);
    
    // Error reporting
    void SetError(const char* format, ...);
};

#endif // MANTLE_VULKAN_RENDERER_H
