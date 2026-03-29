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
struct VkDescriptorSetLayout_T;
struct VkDescriptorPool_T;
struct VkDescriptorSet_T;
struct VkSampler_T;

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
typedef VkDescriptorSetLayout_T* VkDescriptorSetLayout;
typedef VkDescriptorPool_T* VkDescriptorPool;
typedef VkDescriptorSet_T* VkDescriptorSet;
typedef VkSampler_T* VkSampler;

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
    
    // Phase 5d: Frame output and visualization
    std::vector<uint32_t> pixel_buffer_;
    bool enable_frame_dump_;
    uint32_t frame_dump_count_;
    
    // Phase 5f: GPU Buffers for sprite rendering
    VkBuffer quad_vertex_buffer_;       // Static quad vertices (4 vertices)
    VkDeviceMemory quad_vertex_memory_;
    VkBuffer quad_index_buffer_;        // Static quad indices (6 indices: 2 triangles)
    VkDeviceMemory quad_index_memory_;
    VkBuffer instance_buffer_;          // Dynamic instance data (copied per frame)
    VkDeviceMemory instance_buffer_memory_;
    
    // Phase 5h: Staging buffer for instance data updates (host-accessible)
    VkBuffer staging_instance_buffer_;
    VkDeviceMemory staging_instance_memory_;
    
    // Phase 5f: Descriptor sets for texture binding
    VkDescriptorSetLayout descriptor_set_layout_;
    VkDescriptorPool descriptor_pool_;
    std::vector<VkDescriptorSet> descriptor_sets_;  // One per texture ID
    
    // Phase 5f: Placeholder texture for white/fallback
    VkImage placeholder_texture_;
    VkImageView placeholder_texture_view_;
    VkDeviceMemory placeholder_texture_memory_;
    VkSampler texture_sampler_;
    
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
    
    // Phase 5f: GPU Buffer Creation & Management
    bool CreateQuadGeometry();           // Create vertex/index buffers for quad
    bool CreateInstanceBuffer();         // Create dynamic instance data buffer
    bool CreateStagingInstanceBuffer();  // Create host-accessible staging buffer for updates
    bool CreateDescriptorSets();         // Create descriptor set layout and pool
    bool CreatePlaceholderTexture();     // Create white texture fallback
    bool CreateTextureSampler();         // Create VkSampler for texture filtering
    
    // Phase 5g: Buffer Helper Methods
    uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
    bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
                     VkMemoryPropertyFlags properties,
                     VkBuffer& buffer, VkDeviceMemory& memory);
    void CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
    void DestroyBuffer(VkBuffer& buffer, VkDeviceMemory& memory);
    
    // Phase 5f: Instance Data Management
    void UpdateInstanceData(uint32_t sprite_index, const SpritePacket& sprite, 
                           const TransformPacket& transform, uint8_t* data_ptr);
    
    // Phase 5c: Command buffer recording & frame submission
    bool AllocateCommandBuffers();
    bool CreateFence();
    void DestroyFence();
    bool RecordRenderCommands(VkCommandBuffer cmd_buffer, uint32_t image_index, const RenderPacket* packet);
    bool SubmitFrame(uint32_t image_index);
    
    // Helper methods for rendering
    void RecordCommandBuffer(uint32_t image_index, const RenderPacket* packet);
    void RenderTransforms(VkCommandBuffer cmd_buffer, const RenderPacket* packet);
    void RenderSprites(VkCommandBuffer cmd_buffer, const RenderPacket* packet);
    void RenderColliders(VkCommandBuffer cmd_buffer, const RenderPacket* packet);
    
    // Phase 5d: Output and frame dumping
    void DumpCurrentFrame(uint32_t image_index);
    bool SaveFrameToPPM(const char* filename, uint32_t width, uint32_t height);
    
    // Error reporting
    void SetError(const char* format, ...);
};

#endif // MANTLE_VULKAN_RENDERER_H
