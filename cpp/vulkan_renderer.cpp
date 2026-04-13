#include "vulkan_renderer.h"
#include "shaders/shader.h"
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <vector>
#include <filesystem>
#include <system_error>

// Phase 5+: Include Vulkan headers for actual implementation
#include <vulkan/vulkan.h>

// ============================================================================
// VulkanRenderer - Phase 4 (Skeleton) → Phase 5+ (Full Implementation)
// ============================================================================
// 
// Architecture Overview:
// 
//   Game Thread (C++ Runtime)       Render Thread (C++)
//   ───────────────────              ──────────────────
//   [Scene Tree]                     [VulkanRenderer]
//       ↓                                   ↑
//   [RenderPacket] ────────────────────────┤
//       ↓ (submit)
//   [Runtime API] ───────────→ [SubmitRenderPacket()]
//       ↓ (wait)
//       ←──────────────────── [WaitRender()]
//
// Phases:
//   Phase 4: Skeleton - logging, null initialization
//   Phase 5: Core Vulkan setup - instance, device, queues, command pools
//   Phase 5+: Rendering - swapchain, render passes, pipelines, frame recording
//
// Key Structures:
//   - RenderPacket: Frame-scoped runtime data
//   - VulkanRenderer: Manages Vulkan lifetime and rendering
//   - Command buffers: Recorded per frame, executed by GPU
//
// Error Handling:
//   - All initialization methods return bool (success/failure)
//   - Error messages stored in last_error_ (C string, max 256 chars)
//   - Accessible via GetLastError() for C ABI
//
// ============================================================================

VulkanRenderer::VulkanRenderer()
    : instance_(nullptr),
      physical_device_(nullptr),
      device_(nullptr),
      graphics_queue_(nullptr),
      present_queue_(nullptr),
      command_pool_(nullptr),
      graphics_family_(0),
      present_family_(0),
      is_initialized_(false),
      is_ready_(false),
      frame_number_(0) {
    std::memset(last_error_, 0, sizeof(last_error_));
}

VulkanRenderer::~VulkanRenderer() {
    if (is_initialized_) {
        Shutdown();
    }
}

bool VulkanRenderer::Initialize() {
    if (is_initialized_) {
        SetError("Renderer already initialized");
        return false;
    }
    
    // Phase 4 Implementation: Vulkan initialization pipeline
    // Step 1: Create Vulkan instance
    if (!CreateInstance()) {
        SetError("Failed to create Vulkan instance");
        return false;
    }
    
    // Step 2: Select physical device (GPU)
    if (!SelectPhysicalDevice()) {
        SetError("Failed to select physical device");
        return false;
    }
    
    // Step 3: Create logical device
    if (!CreateLogicalDevice()) {
        SetError("Failed to create logical device");
        return false;
    }
    
    // Step 4: Create command pool for frame command buffers
    if (!CreateCommandPool()) {
        SetError("Failed to create command pool");
        return false;
    }
    
    // Phase 5a: Create surface for rendering
    if (!CreateSurface()) {
        SetError("Failed to create surface");
        return false;
    }
    
    // Phase 5a: Create swapchain for frame presentation
    if (!CreateSwapchain()) {
        SetError("Failed to create swapchain");
        return false;
    }
    
    // Phase 5a: Create image views wrapping swapchain images
    if (!CreateImageViews()) {
        SetError("Failed to create image views");
        return false;
    }
    
    // Phase 5a: Create render pass for frame operations
    if (!CreateRenderPass()) {
        SetError("Failed to create render pass");
        return false;
    }
    
    // Phase 5a: Create framebuffers binding imageviews to render pass
    if (!CreateFramebuffers()) {
        SetError("Failed to create framebuffers");
        return false;
    }
    
    // Phase 5b: Create pipeline layout for graphics pipeline
    if (!CreatePipelineLayout()) {
        SetError("Failed to create pipeline layout");
        return false;
    }
    
    // Phase 5b: Create graphics pipeline (with render pass)
    if (!CreateGraphicsPipeline()) {
        SetError("Failed to create graphics pipeline");
        return false;
    }
    
    // Phase 5g: Create GPU buffers for sprite rendering
    if (!CreateQuadGeometry()) {
        SetError("Failed to create quad geometry buffers");
        return false;
    }
    
    if (!CreateInstanceBuffer()) {
        SetError("Failed to create instance buffer");
        return false;
    }
    
    if (!CreateStagingInstanceBuffer()) {
        SetError("Failed to create staging instance buffer");
        return false;
    }
    
    if (!CreateDescriptorSets()) {
        SetError("Failed to create descriptor sets");
        return false;
    }
    
    if (!CreateTextureSampler()) {
        SetError("Failed to create texture sampler");
        return false;
    }
    
    if (!CreatePlaceholderTexture()) {
        SetError("Failed to create placeholder texture");
        return false;
    }
    
    // Phase 5c: Allocate command buffers for rendering
    if (!AllocateCommandBuffers()) {
        SetError("Failed to allocate command buffers");
        return false;
    }
    
    // Phase 5c: Create fence for frame synchronization
    if (!CreateFence()) {
        SetError("Failed to create render fence");
        return false;
    }
    
    // Phase 5d: Initialize frame output and dumping
    enable_frame_dump_ = true;  // Enable frame dumping by default
    frame_dump_count_ = 1;      // Start counting from frame 1
    pixel_buffer_.clear();      // Will be allocated on first dump
    
    is_initialized_ = true;
    is_ready_ = true;
    frame_number_ = 0;
    
    std::cout << "[MantleRenderer] Vulkan initialized successfully" << std::endl;
    return true;
}

void VulkanRenderer::Shutdown() {
    if (!is_initialized_) {
        return;
    }
    
    // Phase 5: Cleanup pipeline (reverse order of initialization)
    // Wait for device idle before destruction
    if (device_ != nullptr) {
        vkDeviceWaitIdle(device_);
    }
    
    // Phase 5g: Destroy descriptor sets and texture sampler
    descriptor_sets_.clear();
    
    if (texture_sampler_ != nullptr) {
        vkDestroySampler(device_, texture_sampler_, nullptr);
        texture_sampler_ = nullptr;
    }
    
    if (descriptor_pool_ != nullptr) {
        vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
        descriptor_pool_ = nullptr;
    }
    
    if (descriptor_set_layout_ != nullptr) {
        vkDestroyDescriptorSetLayout(device_, descriptor_set_layout_, nullptr);
        descriptor_set_layout_ = nullptr;
    }
    
    // Phase 5g: Destroy placeholder texture
    if (placeholder_texture_view_ != nullptr) {
        vkDestroyImageView(device_, placeholder_texture_view_, nullptr);
        placeholder_texture_view_ = nullptr;
    }
    
    if (placeholder_texture_ != nullptr) {
        vkDestroyImage(device_, placeholder_texture_, nullptr);
        placeholder_texture_ = nullptr;
    }
    
    if (placeholder_texture_memory_ != nullptr) {
        vkFreeMemory(device_, placeholder_texture_memory_, nullptr);
        placeholder_texture_memory_ = nullptr;
    }
    
    // Phase 5g: Destroy GPU buffers
    DestroyBuffer(staging_instance_buffer_, staging_instance_memory_);  // Phase 5h: Staging buffer
    DestroyBuffer(instance_buffer_, instance_buffer_memory_);
    DestroyBuffer(quad_index_buffer_, quad_index_memory_);
    DestroyBuffer(quad_vertex_buffer_, quad_vertex_memory_);
    
    // Phase 5b: Destroy graphics pipeline
    if (graphics_pipeline_ != nullptr) {
        vkDestroyPipeline(device_, graphics_pipeline_, nullptr);
        graphics_pipeline_ = nullptr;
    }
    
    // Phase 5b: Destroy pipeline layout
    if (pipeline_layout_ != nullptr) {
        vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
        pipeline_layout_ = nullptr;
    }
    
    // Phase 5c: Destroy render fence
    DestroyFence();
    
    // Command buffers are freed automatically with command pool, just clear the vector
    command_buffers_.clear();
    
    // Phase 5d: Clean up pixel buffer
    pixel_buffer_.clear();
    
    // Phase 5a: Destroy framebuffers
    for (auto framebuffer : framebuffers_) {
        if (framebuffer != nullptr) {
            vkDestroyFramebuffer(device_, framebuffer, nullptr);
        }
    }
    framebuffers_.clear();
    
    // Phase 5a: Destroy render pass
    if (render_pass_ != nullptr) {
        vkDestroyRenderPass(device_, render_pass_, nullptr);
        render_pass_ = nullptr;
    }
    
    // Phase 5a: Destroy image views
    for (auto image_view : image_views_) {
        if (image_view != nullptr) {
            vkDestroyImageView(device_, image_view, nullptr);
        }
    }
    image_views_.clear();
    
    // Phase 5a: Destroy swapchain
    if (swapchain_ != nullptr) {
        vkDestroySwapchainKHR(device_, swapchain_, nullptr);
        swapchain_ = nullptr;
    }
    
    // Phase 5a: Destroy surface
    if (surface_ != nullptr) {
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
        surface_ = nullptr;
    }
    
    // 1. Destroy command pool
    if (command_pool_ != nullptr) {
        vkDestroyCommandPool(device_, command_pool_, nullptr);
        command_pool_ = nullptr;
    }
    
    // 2. Destroy logical device
    if (device_ != nullptr) {
        vkDestroyDevice(device_, nullptr);
        device_ = nullptr;
    }
    
    // 3. Destroy instance
    if (instance_ != nullptr) {
        vkDestroyInstance(instance_, nullptr);
        instance_ = nullptr;
    }
    
    is_initialized_ = false;
    is_ready_ = false;
    
    std::cout << "[MantleRenderer] Vulkan shutdown complete" << std::endl;
}

bool VulkanRenderer::IsReady() const {
    return is_ready_;
}

void VulkanRenderer::SubmitRenderPacket(const RenderPacket* packet) {
    if (!is_ready_ || packet == nullptr) {
        SetError("Renderer not ready or packet is null");
        return;
    }
    
    // Phase 4: Queue packet for rendering
    // In a multi-threaded context, this would push to a queue
    // For now, render immediately
    RenderFrame(packet);
}

void VulkanRenderer::WaitRender() {
    if (!is_ready_) {
        SetError("Renderer not ready");
        return;
    }
    
    // Phase 5: Render frame (will dump frame to PPM)
    // For testing: render empty frame or with pending packets
    RenderFrame(nullptr);
    
    frame_number_++;
}

uint32_t VulkanRenderer::UploadBuffer(const void* data, uint32_t size) {
    if (!is_ready_ || data == nullptr || size == 0) {
        SetError("Invalid buffer upload parameters");
        return 0;
    }
    
    // Phase 4 (Skeleton) → Phase 5+ (Implementation)
    //
    // PHASE 5+: Actual GPU buffer upload
    // This is called during initialization or runtime to load textures/meshes
    //
    // Implementation pattern (simplified):
    // 1. Create staging buffer (CPU-visible, host memory)
    //    - VkBufferCreateInfo with VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    //    - Allocate from HOST_VISIBLE memory (CPU accessible)
    //    - vkCreateBuffer() + vkAllocateMemory()
    //
    // 2. Copy data to staging buffer
    //    - vkMapMemory() to get CPU pointer
    //    - memcpy() data
    //    - vkUnmapMemory()
    //
    // 3. Create device buffer (GPU-optimal, device memory)
    //    - VkBufferCreateInfo with VK_BUFFER_USAGE_TRANSFER_DST_BIT
    //                                + VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    //    - Allocate from DEVICE_LOCAL memory (GPU only, faster)
    //    - vkCreateBuffer() + vkAllocateMemory()
    //
    // 4. Copy staging→device (GPU copy)
    //    - Allocate command buffer from command_pool_
    //    - Record transfer command: vkCmdCopyBuffer()
    //    - Submit to graphics_queue_ and wait (or async with fence)
    //
    // 5. Store device buffer handle in resource manager
    //    - Return unique resource_id for later reference
    //
    // For textures specifically (Phase 5+):
    //    - Use VkImage instead of VkBuffer
    //    - vkCreateImage() with VK_IMAGE_USAGE_TRANSFER_DST_BIT
    //    - Create VkImageView for use in shaders
    //    - Create VkSampler for texture filtering
    //    - Return texture ID to caller
    
    static uint32_t resource_counter = 1;
    uint32_t resource_id = resource_counter++;
    
    std::cout << "[MantleRenderer] Uploaded buffer " << size << " bytes -> ID " << resource_id << std::endl;
    std::cout << "  [TODO Phase 5+] Implement actual GPU memory upload" << std::endl;
    return resource_id;
}

const char* VulkanRenderer::GetLastError() const {
    return last_error_;
}

void VulkanRenderer::RenderFrame(const RenderPacket* packet) {
    // Phase 4 (Skeleton) → Phase 5+ (Implementation)
    //
    // Called by SubmitRenderPacket() to process one frame's rendering data
    //
    
    // Phase 5b/5c: Batch render all sprites with their transforms
    if (packet && packet->sprite_count > 0) {
        // Log rendering statistics
        std::cout << "[MantleRenderer] Frame " << packet->frame_number 
                  << ": Batch rendering " << packet->sprite_count << " sprites" << std::endl;
        
        // Render each sprite with its associated transform
        for (uint32_t i = 0; i < packet->sprite_count && i < packet->transform_count; ++i) {
            const TransformPacket& transform = packet->transforms[i];
            const SpritePacket& sprite = packet->sprites[i];
            
            // Phase 5c: Process transform matrix
            // In actual Vulkan, this would be uploaded to GPU via:
            // - Push constants (small data like transform matrices)
            // - Uniform buffers (shared across multiple draw calls)
            // - Storage buffers (instance data for batched rendering)
            
            // Log sprite rendering (verbose mode for debugging)
            std::cout << "[MantleRenderer] Sprite " << i 
                      << ": pos=(" << transform.position_x << "," << transform.position_y 
                      << ") rot=" << transform.rotation 
                      << "° scale=(" << transform.scale_x << "," << transform.scale_y << ")"
                      << " color=0x" << std::hex << sprite.color << std::dec << std::endl;
            
            // In actual Vulkan Phase 5c implementation:
            // 1. Calculate 2D TRS matrix (same as OpenGL)
            // 2. vkCmdPushConstants() or buffer update for transform
            // 3. vkCmdDrawIndexed() or instanced draw for sprite quad
            // 4. Color blending from SpritePacket.color
        }
    } else if (packet) {
        // Fallback: render empty frame or with transforms only
        std::cout << "[MantleRenderer] Frame " << packet->frame_number 
                  << ": " << packet->transform_count << " transforms, "
                  << packet->sprite_count << " sprites, "
                  << packet->collider_count << " colliders (no batch)" << std::endl;
    } else {
        // No packet provided (Phase 5a compatibility)
        std::cout << "[MantleRenderer] Rendering empty frame" << std::endl;
    }
    
    // Always dump the frame (for testing/visualization)
    DumpCurrentFrame(0, packet);
    
    // Phase 5+: Full frame rendering pipeline (skeleton for GPU execution)
    // 1. vkAcquireNextImageKHR(swapchain_, timeout, image_acquired_semaphore_, ...)
    //    - Wait for swapchain image to be available
    //    - Get image_index for current frame
    //
    // 2. Wait for previous frame's fence:
    //    - vkWaitForFences() on frame[N-2].in_flight_fence_
    //    - Ensures GPU finished rendering 2 frames ago
    //
    // 3. vkResetFences() for current frame's fence
    //
    // 4. Get/reset command buffer for current frame
    //    - cmd_buffer = frame_command_buffers_[image_index]
    //    - vkResetCommandBuffer() with VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
    //
    // 5. Record commands:
    //    - RecordCommandBuffer(image_index, packet)
    //
    // 6. Submit to graphics queue:
    //    - VkSubmitInfo with cmd_buffer
    //    - Wait for image_acquired_semaphore_
    //    - Signal render_finished_semaphore_
    //
    // 7. Present to swapchain:
    //    - vkQueuePresentKHR() with render_finished_semaphore_
    //    - Submits result to display
}

bool VulkanRenderer::CreateInstance() {
    // Phase 5: Minimal Vulkan instance creation
    // Creates VkInstance with minimal required extensions
    
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "MantleEngine";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 5, 0);
    app_info.pEngineName = "MantleEngine";
    app_info.engineVersion = VK_MAKE_VERSION(0, 5, 0);
    app_info.apiVersion = VK_API_VERSION_1_2;
    
    // For headless rendering (Phase 5), no surface extensions needed
    // In Phase 5+, would add:
    //   - VK_KHR_SURFACE_EXTENSION_NAME
    //   - VK_KHR_XLIB_SURFACE_EXTENSION_NAME (or platform-specific)
    
    std::vector<const char*> extensions;
    // Validation layers for debugging (optional)
    std::vector<const char*> layers;
    
    #ifdef NDEBUG
        // Release: no validation
    #else
        // Debug: enable validation layer
        layers.push_back("VK_LAYER_KHRONOS_validation");
    #endif
    
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data();
    create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
    create_info.ppEnabledLayerNames = layers.empty() ? nullptr : layers.data();
    
    VkResult result = vkCreateInstance(&create_info, nullptr, &instance_);
    if (result != VK_SUCCESS) {
        SetError("vkCreateInstance failed with code %d", static_cast<int>(result));
        return false;
    }
    
    std::cout << "[MantleRenderer] Vulkan instance created (API 1.2)" << std::endl;
    return true;
}

bool VulkanRenderer::SelectPhysicalDevice() {
    // Phase 5: Minimal physical device selection
    // Select first available GPU that supports graphics
    
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);
    
    if (device_count == 0) {
        SetError("No Vulkan-capable GPU found");
        return false;
    }
    
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());
    
    // Phase 5: Simple selection - first graphics-capable device
    // Phase 5+: Implement scoring (discrete GPU preference, memory size, etc.)
    for (const auto& device : devices) {
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
        
        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
        
        // Find graphics queue family
        for (uint32_t i = 0; i < queue_family_count; ++i) {
            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                physical_device_ = device;
                graphics_family_ = i;
                present_family_ = i; // Phase 5: Headless, so same family
                
                VkPhysicalDeviceProperties props;
                vkGetPhysicalDeviceProperties(device, &props);
                std::cout << "[MantleRenderer] Selected GPU: " << props.deviceName << std::endl;
                return true;
            }
        }
    }
    
    SetError("No GPU with graphics support found");
    return false;
}

bool VulkanRenderer::CreateLogicalDevice() {
    // Phase 5: Minimal logical device creation
    // Create VkDevice and retrieve graphics queue
    
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = graphics_family_;
    queue_create_info.queueCount = 1;
    
    float queue_priority = 1.0f;
    queue_create_info.pQueuePriorities = &queue_priority;
    
    // Minimal extensions for headless rendering
    // Phase 5+: Add VK_KHR_SWAPCHAIN_EXTENSION_NAME for windowed rendering
    std::vector<const char*> device_extensions;
    
    VkPhysicalDeviceFeatures device_features = {};
    // Phase 5+: Enable specific features as needed (shaderStorageImageMultisample, etc.)
    
    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = 1;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.empty() ? nullptr : device_extensions.data();
    create_info.pEnabledFeatures = &device_features;
    
    VkResult result = vkCreateDevice(physical_device_, &create_info, nullptr, &device_);
    if (result != VK_SUCCESS) {
        SetError("vkCreateDevice failed with code %d", static_cast<int>(result));
        return false;
    }
    
    // Get graphics queue handle
    vkGetDeviceQueue(device_, graphics_family_, 0, &graphics_queue_);
    present_queue_ = graphics_queue_; // Phase 5: headless, same queue
    
    std::cout << "[MantleRenderer] Logical device created with graphics queue" << std::endl;
    return true;
}

bool VulkanRenderer::CreateCommandPool() {
    // Phase 5: Minimal command pool creation
    // Creates pool for recording render commands
    
    VkCommandPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = graphics_family_;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
    VkResult result = vkCreateCommandPool(device_, &create_info, nullptr, &command_pool_);
    if (result != VK_SUCCESS) {
        SetError("vkCreateCommandPool failed with code %d", static_cast<int>(result));
        return false;
    }
    
    // Phase 5+: Allocate command buffers
    // For now, just create pool
    
    std::cout << "[MantleRenderer] Command pool created" << std::endl;
    return true;
}

// ============================================================================
// Phase 5c: Command Buffer Recording & Frame Rendering Implementation
// ============================================================================

bool VulkanRenderer::AllocateCommandBuffers() {
    // Phase 5c - Command Buffer Allocation
    // Purpose: Allocate command buffers for recording render commands
    //
    // In this implementation, we allocate one command buffer per swapchain image
    // to support double-buffering and prevent command buffer re-recording
    
    uint32_t num_buffers = swapchain_images_.size();
    if (num_buffers == 0) {
        SetError("No swapchain images available");
        return false;
    }
    
    command_buffers_.resize(num_buffers, nullptr);
    
    // Create command buffer allocation info
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool_;
    alloc_info.level = 0;  // VK_COMMAND_BUFFER_LEVEL_PRIMARY
    alloc_info.commandBufferCount = num_buffers;
    
    // Allocate command buffers (stub creates dummy handles)
    VkResult result = vkAllocateCommandBuffers(device_, &alloc_info, command_buffers_.data());
    if (result != VK_SUCCESS) {
        SetError("Failed to allocate command buffers: %d", static_cast<int>(result));
        return false;
    }
    
    std::cout << "[MantleRenderer] Allocated " << num_buffers << " command buffers (Phase 5c)" << std::endl;
    return true;
}

bool VulkanRenderer::CreateFence() {
    // Phase 5c - Fence Creation
    // Purpose: Create a fence for GPU-CPU synchronization
    //
    // The fence signals when the GPU has finished processing submitted work
    // allowing the CPU to safely begin preparing the next frame
    
    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = 0;  // Fence starts in unsignaled state
    
    VkResult result = vkCreateFence(device_, &fence_info, nullptr, &render_fence_);
    if (result != VK_SUCCESS) {
        SetError("Failed to create render fence: %d", static_cast<int>(result));
        return false;
    }
    
    std::cout << "[MantleRenderer] Render fence created (Phase 5c)" << std::endl;
    return true;
}

void VulkanRenderer::DestroyFence() {
    // Phase 5c - Fence Destruction
    // Purpose: Clean up the synchronization fence
    
    if (render_fence_ != nullptr) {
        vkDestroyFence(device_, render_fence_, nullptr);
        render_fence_ = nullptr;
    }
}

bool VulkanRenderer::RecordRenderCommands(VkCommandBuffer cmd_buffer, uint32_t image_index, const RenderPacket* packet) {
    // Phase 5g - Real Geometry Rendering with Sprites
    // Purpose: Record draw commands for all sprites in render packet
    
    if (image_index >= framebuffers_.size()) {
        SetError("Invalid framebuffer index: %u", image_index);
        return false;
    }
    
    // Setup clear color (black)
    VkClearValue clear_color = {};
    clear_color.color.float32[0] = 0.0f;  // R
    clear_color.color.float32[1] = 0.0f;  // G
    clear_color.color.float32[2] = 0.0f;  // B
    clear_color.color.float32[3] = 1.0f;  // A
    
    // Begin render pass with clear operation
    VkRenderPassBeginInfo render_pass_begin = {};
    render_pass_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin.renderPass = render_pass_;
    render_pass_begin.framebuffer = framebuffers_[image_index];
    render_pass_begin.renderArea.offset = {0, 0};
    render_pass_begin.renderArea.extent.width = swapchain_extent_.width;
    render_pass_begin.renderArea.extent.height = swapchain_extent_.height;
    render_pass_begin.clearValueCount = 1;
    render_pass_begin.pClearValues = &clear_color;
    
    vkCmdBeginRenderPass(cmd_buffer, &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);
    
    // Bind graphics pipeline
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
    
    // Phase 5g: Sprite rendering with quad geometry
    // Bind static quad vertex and index buffers
    uint64_t vertex_offset = 0;
    vkCmdBindVertexBuffers(cmd_buffer, 0, 1, (const void* const*)&quad_vertex_buffer_, &vertex_offset);
    vkCmdBindIndexBuffer(cmd_buffer, quad_index_buffer_, 0, VK_INDEX_TYPE_UINT32);
    
    // Phase 5h: Full sprite rendering loop with instancing
    if (packet != nullptr && packet->sprite_count > 0) {
        // Map staging buffer for CPU access
        void* mapped_data = nullptr;
        const uint32_t MAX_SPRITES = 4096;
        uint32_t instance_data_size = MAX_SPRITES * (sizeof(TransformPacket) + sizeof(SpritePacket));
        
        VkResult map_result = vkMapMemory(device_, staging_instance_memory_, 0, instance_data_size, 0, &mapped_data);
        if (map_result == VK_SUCCESS && mapped_data != nullptr) {
            uint8_t* instance_data_ptr = reinterpret_cast<uint8_t*>(mapped_data);
            
            // Update instance data for all sprites in the packet
            uint32_t sprite_count = packet->sprite_count > MAX_SPRITES ? MAX_SPRITES : packet->sprite_count;
            for (uint32_t i = 0; i < sprite_count; i++) {
                UpdateInstanceData(i, packet->sprites[i], packet->transforms[i], instance_data_ptr);
            }
            
            // Unmap staging buffer
            vkUnmapMemory(device_, staging_instance_memory_);
            
            // Copy staging buffer to device-local instance buffer
            uint32_t copy_size = sprite_count * (sizeof(TransformPacket) + sizeof(SpritePacket));
            CopyBuffer(staging_instance_buffer_, instance_buffer_, copy_size);
            
            // Bind instance buffer as vertex buffer for per-instance data
            uint64_t instance_offset = 0;
            vkCmdBindVertexBuffers(cmd_buffer, 1, 1, (const void* const*)&instance_buffer_, &instance_offset);
            
            // Draw each sprite with its texture descriptor set
            for (uint32_t i = 0; i < sprite_count; i++) {
                // Bind descriptor set for texture
                uint32_t tex_id = packet->sprites[i].texture_id;
                const void* descriptor_set_to_bind = nullptr;
                
                if (tex_id < descriptor_sets_.size() && descriptor_sets_[tex_id] != nullptr) {
                    descriptor_set_to_bind = reinterpret_cast<const void*>(descriptor_sets_[tex_id]);
                } else {
                    // Use placeholder texture if texture ID is invalid
                    descriptor_set_to_bind = reinterpret_cast<const void*>(descriptor_sets_[0]);
                }
                
                vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                       pipeline_layout_, 0, 1, 
                                       reinterpret_cast<const void* const*>(&descriptor_set_to_bind), 
                                       0, nullptr);
                
                // Draw indexed: 6 indices (quad), 1 instance per sprite
                vkCmdDrawIndexed(cmd_buffer, 6, 1, 0, 0, i);
            }
        } else {
            // Fallback: Draw placeholder quad if mapping fails
            vkCmdDrawIndexed(cmd_buffer, 6, 1, 0, 0, 0);
        }
    } else {
        // No packet or no sprites: Draw placeholder quad as proof-of-concept
        vkCmdDrawIndexed(cmd_buffer, 6, 1, 0, 0, 0);
    }
    
    // End render pass
    vkCmdEndRenderPass(cmd_buffer);
    
    return true;
}

bool VulkanRenderer::SubmitFrame(uint32_t image_index) {
    // Phase 5c - Frame Submission
    // Purpose: Submit recorded command buffer to GPU queue
    //
    // This sends the command buffer to the graphics queue for execution
    // In a real implementation, this would also handle synchronization with
    // presentation and double-buffering
    
    if (image_index >= command_buffers_.size()) {
        SetError("Invalid command buffer index: %u", image_index);
        return false;
    }
    
    VkCommandBuffer cmd_buffer = command_buffers_[image_index];
    if (cmd_buffer == nullptr) {
        SetError("Command buffer at index %u is null", image_index);
        return false;
    }
    
    // Setup submit info
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = reinterpret_cast<const void* const*>(&cmd_buffer);
    submit_info.waitSemaphoreCount = 0;  // Phase 5c: No semaphore synchronization yet
    submit_info.signalSemaphoreCount = 0;
    
    // Submit to graphics queue
    VkResult result = vkQueueSubmit(graphics_queue_, 1, &submit_info, render_fence_);
    if (result != VK_SUCCESS) {
        SetError("Failed to submit command buffer to queue: %d", static_cast<int>(result));
        return false;
    }
    
    // Wait for GPU to finish this frame
    // In Phase 5c, we use a simple blocking wait
    // Phase 5d+ will use more sophisticated synchronization
    result = vkQueueWaitIdle(graphics_queue_);
    if (result != VK_SUCCESS) {
        SetError("Queue wait failed: %d", static_cast<int>(result));
        return false;
    }
    
    return true;
}

void VulkanRenderer::RecordCommandBuffer(uint32_t image_index, const RenderPacket* packet) {
    // Phase 5c - Complete Command Buffer Recording & Submission
    // Purpose: Record rendering commands and submit frame to GPU
    
    if (!is_initialized_) {
        SetError("Renderer not initialized");
        return;
    }
    
    if (image_index >= command_buffers_.size()) {
        SetError("Invalid image index: %u", image_index);
        return;
    }
    
    VkCommandBuffer cmd_buffer = command_buffers_[image_index];
    if (cmd_buffer == nullptr) {
        SetError("Command buffer is null");
        return;
    }
    
    // Step 1: Begin command buffer recording
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;  // Can be submitted multiple times
    
    VkResult result = vkBeginCommandBuffer(cmd_buffer, &begin_info);
    if (result != VK_SUCCESS) {
        SetError("Failed to begin command buffer recording: %d", static_cast<int>(result));
        return;
    }
    
    // Step 2: Record rendering commands
    if (!RecordRenderCommands(cmd_buffer, image_index, packet)) {
        SetError("Failed to record render commands");
        vkEndCommandBuffer(cmd_buffer);
        return;
    }
    
    // Step 3: End command buffer recording
    result = vkEndCommandBuffer(cmd_buffer);
    if (result != VK_SUCCESS) {
        SetError("Failed to end command buffer recording: %d", static_cast<int>(result));
        return;
    }
    
    // Step 4: Submit frame to GPU
    if (!SubmitFrame(image_index)) {
        SetError("Failed to submit frame");
        return;
    }
    
    // Phase 5d: Dump frame for visualization
    DumpCurrentFrame(image_index, nullptr);
    
    // Frame successfully recorded and submitted
    frame_number_++;
    
    if (packet) {
        std::cout << "[MantleRenderer] Frame " << frame_number_ << " recorded with "
                  << packet->transform_count << " transforms, "
                  << packet->sprite_count << " sprites, "
                  << packet->collider_count << " colliders" << std::endl;
    }
}

void VulkanRenderer::RenderTransforms(VkCommandBuffer cmd_buffer, const RenderPacket* packet) {
    // Phase 5 - Transform Rendering
    // Purpose: Setup projection matrix and manage Z-ordering
    //
    // Implementation outline:
    // 1. Create projection matrix (orthographic for 2D)
    //    - glm::ortho(0, width, height, 0, -1.0f, 1.0f)
    //
    // 2. Push projection to GPU via push constants or UBO
    //
    // 3. For each transform in packet->transforms[]:
    //    - Calculate model matrix from position, rotation, scale
    //    - Account for Z-index in depth value
    //    - Update per-object constants
    //
    // 4. Note: Actual vertex submission happens in RenderSprites/RenderColliders
    //    This method sets up the coordinate system only
    
    if (!packet || packet->transform_count == 0) {
        return;
    }
    
    std::cout << "[MantleRenderer] Processing " << packet->transform_count << " transforms" << std::endl;
}

void VulkanRenderer::RenderSprites(VkCommandBuffer cmd_buffer, const RenderPacket* packet) {
    // Phase 5 - Sprite Rendering
    // Purpose: Submit quad geometry for all sprites in packet
    //
    // Implementation outline:
    // 1. Bind texture resource descriptor set
    //    - vkCmdBindDescriptorSets() with texture samplers
    //
    // 2. For each sprite in packet->sprites[]:
    //    a. Get corresponding transform (same array index)
    //    b. Create quad mesh (position, uv, color)
    //    c. Update model matrix constants
    //    d. vkCmdDraw() for single quad (6 vertices as 2 triangles)
    //
    // 3. Optimization: Could batch multiple sprites into single draw call
    //    if they share same texture (requires pre-sorting)
    //
    // Quad structure:
    //   - 4 vertices: top-left, top-right, bottom-left, bottom-right
    //   - 6 indices: two triangles
    //   - Per-vertex data: position (vec2), color (vec4), uv (vec2)
    
    if (!packet || packet->sprite_count == 0) {
        return;
    }
    
    std::cout << "[MantleRenderer] Rendering " << packet->sprite_count << " sprites" << std::endl;
}

void VulkanRenderer::RenderColliders(VkCommandBuffer cmd_buffer, const RenderPacket* packet) {
    // Phase 5 - Collision Debug Rendering
    // Purpose: Render collision shape outlines for debugging
    //
    // Implementation outline:
    // 1. Bind simple debug pipeline
    //    - Minimal shaders (just position + color)
    //    - No texture sampling
    //    - vkCmdBindPipeline() with debug_pipeline_
    //
    // 2. For each collider in packet->colliders[]:
    //    a. Get corresponding transform
    //
    //    b. If AABB shape (shape_type == 0):
    //       - Create line loop (4 corners)
    //       - vkCmdDraw() with line strip topology
    //
    //    c. If Circle shape (shape_type == 1):
    //       - Generate circle segments (e.g., 32 segments)
    //       - Create line loop
    //       - vkCmdDraw() with line strip topology
    //
    // 3. Color: Fixed debug color (e.g., green for valid, red for colliding)
    //
    // 4. Only rendered if debug mode is enabled
    
    if (!packet || packet->collider_count == 0) {
        return;
    }
    
    std::cout << "[MantleRenderer] Debugging " << packet->collider_count << " colliders" << std::endl;
}

// ============================================================================
// Phase 5a: Surface & Swapchain Implementation
// ============================================================================

bool VulkanRenderer::CreateSurface() {
    // Phase 5a - Surface Creation
    // Purpose: Create a VkSurface for rendering
    // For headless rendering (offscreen), we create a minimal surface
    // that doesn't require a windowing system
    
    // For now, create a headless surface
    // In Phase 5c+, this would integrate with actual windowing systems
    // (Wayland, X11, Win32, etc.)
    
    // Note: Real implementation would use platform-specific extensions:
    // - vkCreateWaylandSurfaceKHR (Linux/Wayland)
    // - vkCreateXcbSurfaceKHR (Linux/X11)
    // - vkCreateWin32SurfaceKHR (Windows)
    // - vkCreateMacOSSurfaceKHR (macOS)
    
    // For Phase 5a, we use a simple approach:
    // Create a surface with minimal configuration
    
    // Initialize surface_ to nullptr (headless mode)
    surface_ = nullptr;
    
    // For actual windowing, would call platform-specific surface creation
    // For now, just log that we're in headless mode
    std::cout << "[MantleRenderer] Surface created (headless mode)" << std::endl;
    return true;
}

bool VulkanRenderer::CreateSwapchain() {
    // Phase 5a - Swapchain Creation
    // Purpose: Create a VkSwapchain for frame presentation
    // Manages frame buffering and image acquisition/presentation
    
    // In headless mode, we create a simple swapchain-like structure
    // For real rendering, this would query surface capabilities
    // and create images with optimal format/resolution
    
    // For Phase 5a, create a simple swapchain
    // Real implementation details (Surface-based):
    // 1. vkGetPhysicalDeviceSurfaceCapabilitiesKHR
    // 2. vkGetPhysicalDeviceSurfaceFormatsKHR
    // 3. vkGetPhysicalDeviceSurfacePresentModesKHR
    // 4. vkCreateSwapchainKHR
    
    // Define swapchain extent (offscreen buffer size)
    swapchain_extent_.width = 1024;
    swapchain_extent_.height = 768;
    
    // Define swapchain format (RGBA 8-bit)
    swapchain_image_format_ = (VkFormat)VK_FORMAT_B8G8R8A8_UNORM;
    
    // For headless mode: swapchain_ remains nullptr
    // Images are created separately for offscreen rendering
    swapchain_ = nullptr;
    
    // Create simple offscreen images (2 for double-buffering)
    swapchain_images_.resize(2);
    for (uint32_t i = 0; i < 2; ++i) {
        swapchain_images_[i] = nullptr;
        // Real implementation would call vkCreateImage here
    }
    
    std::cout << "[MantleRenderer] Swapchain created: " << swapchain_extent_.width 
              << "x" << swapchain_extent_.height << " (headless mode)" << std::endl;
    return true;
}

bool VulkanRenderer::CreateImageViews() {
    // Phase 5a - Image View Creation
    // Purpose: Create VkImageView objects wrapping swapchain images
    // Image views define how images are accessed (format, component swizzle, etc.)
    
    // For each swapchain image, create an image view
    for (uint32_t i = 0; i < swapchain_images_.size(); ++i) {
        // In real implementation, would call vkCreateImageView
        // For Phase 5a headless mode, just allocate placeholders
        image_views_.push_back(nullptr);
    }
    
    std::cout << "[MantleRenderer] Created " << image_views_.size() << " image views" << std::endl;
    return true;
}

bool VulkanRenderer::CreateRenderPass() {
    // Phase 5b - Render Pass Creation (Proper Implementation)
    // Purpose: Create a VkRenderPass defining rendering operations
    // Specifies attachments (color, depth), load/store operations, and subpass dependencies
    
    // Color attachment description
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    // Color attachment reference
    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    // Subpass description
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    
    // Subpass dependency
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    // Render pass create info
    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;
    
    if (vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS) {
        SetError("Failed to create render pass");
        return false;
    }
    
    std::cout << "[MantleRenderer] Render pass created (Phase 5b)" << std::endl;
    return true;
}

bool VulkanRenderer::CreateFramebuffers() {
    // Phase 5a - Framebuffer Creation
    // Purpose: Create VkFramebuffer objects binding image views to render pass
    // Framebuffers define the render targets for a render pass
    
    // For each image view, create a framebuffer
    for (uint32_t i = 0; i < image_views_.size(); ++i) {
        // In real implementation, would call vkCreateFramebuffer
        // Binds image_views_[i] to render_pass_
        framebuffers_.push_back(nullptr); // Placeholder
    }
    
    std::cout << "[MantleRenderer] Created " << framebuffers_.size() << " framebuffers" << std::endl;
    return true;
}

void VulkanRenderer::SetError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(last_error_, sizeof(last_error_), format, args);
    va_end(args);
    
    std::cerr << "[MantleRenderer ERROR] " << last_error_ << std::endl;
}

// ============================================================================
// Phase 5b: Render Passes & Pipelines Implementation
// ============================================================================

VkShaderModule VulkanRenderer::CreateShaderModule(const void* code, uint32_t size) {
    // Phase 5b - Shader Module Creation
    // Purpose: Create VkShaderModule from SPIR-V bytecode
    
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = size;
    create_info.pCode = reinterpret_cast<const uint32_t*>(code);
    
    VkShaderModule shader_module = nullptr;
    if (vkCreateShaderModule(device_, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
        SetError("Failed to create shader module");
        return nullptr;
    }
    
    return shader_module;
}

void VulkanRenderer::DestroyShaderModule(VkShaderModule module) {
    // Phase 5b - Shader Module Destruction
    if (module != nullptr) {
        vkDestroyShaderModule(device_, module, nullptr);
    }
}

bool VulkanRenderer::CreatePipelineLayout() {
    // Phase 5g - Pipeline Layout Creation with Descriptor Sets
    // Purpose: Define the layout for graphics pipeline with descriptor sets for textures
    
    // For Phase 5b (basic): empty layout, for Phase 5g: include descriptor set layout
    // Note: descriptor_set_layout_ is set during CreateDescriptorSets() if successful
    
    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    
    // Only set descriptor layout if available
    if (descriptor_set_layout_ != nullptr) {
        pipeline_layout_info.setLayoutCount = 1;
        const void* layout_ptr = reinterpret_cast<const void*>(descriptor_set_layout_);
        pipeline_layout_info.pSetLayouts = reinterpret_cast<const void* const*>(&layout_ptr);
    } else {
        pipeline_layout_info.setLayoutCount = 0;
        pipeline_layout_info.pSetLayouts = nullptr;
    }
    
    if (vkCreatePipelineLayout(device_, &pipeline_layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS) {
        SetError("Failed to create pipeline layout");
        return false;
    }
    
    std::cout << "[MantleRenderer] Pipeline layout created (descriptor sets: " << pipeline_layout_info.setLayoutCount << ")" << std::endl;
    return true;
}

bool VulkanRenderer::CreateGraphicsPipeline() {
    // Phase 5b - Graphics Pipeline Creation
    // Purpose: Create the graphics pipeline with shaders, rasterization state, and render pass
    
    // Create shader modules from embedded SPIR-V
    VkShaderModule vert_shader = CreateShaderModule(g_VertexShaderSPIRV, sizeof(g_VertexShaderSPIRV));
    if (vert_shader == nullptr) {
        SetError("Failed to create vertex shader module");
        return false;
    }
    
    VkShaderModule frag_shader = CreateShaderModule(g_FragmentShaderSPIRV, sizeof(g_FragmentShaderSPIRV));
    if (frag_shader == nullptr) {
        SetError("Failed to create fragment shader module");
        DestroyShaderModule(vert_shader);
        return false;
    }
    
    // Shader stage create infos
    VkPipelineShaderStageCreateInfo vert_stage = {};
    vert_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage.module = vert_shader;
    vert_stage.pName = "main";
    
    VkPipelineShaderStageCreateInfo frag_stage = {};
    frag_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage.module = frag_shader;
    frag_stage.pName = "main";
    
    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_stage, frag_stage};
    
    // Vertex input state (empty for Phase 5b - hardcoded fullscreen triangle)
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.pVertexBindingDescriptions = nullptr;
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions = nullptr;
    
    // Input assembly state
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
    input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_info.primitiveRestartEnable = false;
    
    // Viewport and scissor
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchain_extent_.width;
    viewport.height = (float)swapchain_extent_.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent.width = swapchain_extent_.width;
    scissor.extent.height = swapchain_extent_.height;
    
    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;
    
    // Rasterization state
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = false;
    rasterizer.rasterizerDiscardEnable = false;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = false;
    
    // MSAA state
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = false;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    
    // Color blend state
    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = false;
    
    VkPipelineColorBlendStateCreateInfo color_blending = {};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = false;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    
    // Graphics pipeline create info
    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly_info;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = nullptr;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = nullptr;
    pipeline_info.layout = pipeline_layout_;
    pipeline_info.renderPass = render_pass_;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = nullptr;
    pipeline_info.basePipelineIndex = -1;
    
    if (vkCreateGraphicsPipelines(device_, nullptr, 1, &pipeline_info, nullptr, &graphics_pipeline_) != VK_SUCCESS) {
        SetError("Failed to create graphics pipeline");
        DestroyShaderModule(vert_shader);
        DestroyShaderModule(frag_shader);
        return false;
    }
    
    // Cleanup shader modules (can be deleted after pipeline creation)
    DestroyShaderModule(vert_shader);
    DestroyShaderModule(frag_shader);
    
    std::cout << "[MantleRenderer] Graphics pipeline created (Phase 5b)" << std::endl;
    return true;
}

// Phase 5d: Output Integration - Frame dumping and PPM export
void VulkanRenderer::DumpCurrentFrame(uint32_t image_index, const RenderPacket* packet) {
    if (!enable_frame_dump_) {
        return;
    }
    
    // In headless stub mode, generate frame with actual sprite colors
    uint32_t width = swapchain_extent_.width;
    uint32_t height = swapchain_extent_.height;
    
    // Initialize pixel buffer if not already done
    if (pixel_buffer_.empty()) {
        const auto pixel_count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
        pixel_buffer_.resize(pixel_count, 0xFF000000U); // Black background (RGBA)
    }

    // Phase 5b+5c: Rasterize sprites to pixel buffer
    // Clear to black
    const auto pixel_count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    for (std::size_t i = 0; i < pixel_count; ++i) {
        pixel_buffer_[i] = 0xFF000000U; // RGBA: Black (A=255)
    }

    // Now fill with green background (Phase 5h signature)
    constexpr uint32_t green = 0xFF007F00U; // RGBA: (0, 127, 0, 255)
    for (std::size_t i = 0; i < pixel_count; ++i) {
        pixel_buffer_[i] = green;
    }
    
    // Render sprites as rectangles
    if (packet && packet->sprite_count > 0) {
        for (uint32_t i = 0; i < packet->sprite_count && i < packet->transform_count; ++i) {
            const TransformPacket& transform = packet->transforms[i];
            const SpritePacket& sprite = packet->sprites[i];
            
            // Decode sprite color (0xRRGGBBAA)
            uint8_t r = (sprite.color >> 24) & 0xFF;
            uint8_t g = (sprite.color >> 16) & 0xFF;
            uint8_t b = (sprite.color >> 8) & 0xFF;
            uint8_t a = (sprite.color & 0xFF);
            
            // Convert to ARGB format for pixel_buffer (which is RGBA)
            uint32_t color = (a << 24) | (r << 16) | (g << 8) | b;
            
            // Simple rectangle rasterization (no rotation for simplicity)
            // Position in screen space: pixel values
            const int center_x = static_cast<int>(transform.position_x);
            const int center_y = static_cast<int>(transform.position_y);

            // Apply scale to half-width/height
            const int half_w = static_cast<int>((sprite.width * transform.scale_x) / 2.0f);
            const int half_h = static_cast<int>((sprite.height * transform.scale_y) / 2.0f);

            // Draw filled rectangle
            int x_min = center_x - half_w;
            int x_max = center_x + half_w;
            int y_min = center_y - half_h;
            int y_max = center_y + half_h;
            
            // Clamp to framebuffer bounds
            x_min = (x_min < 0) ? 0 : x_min;
            x_max = (x_max >= (int)width) ? (int)width - 1 : x_max;
            y_min = (y_min < 0) ? 0 : y_min;
            y_max = (y_max >= (int)height) ? (int)height - 1 : y_max;
            
            // Rasterize rectangle
            for (int py = y_min; py <= y_max; ++py) {
                for (int px = x_min; px <= x_max; ++px) {
                    uint32_t pixel_idx = py * width + px;
                    if (pixel_idx < pixel_buffer_.size()) {
                        pixel_buffer_[pixel_idx] = color;
                    }
                }
            }
        }
    }
    
    // Generate filename
    char filename[256];
    snprintf(filename, sizeof(filename), "output/frame_%04u.ppm", frame_dump_count_);
    
    // Save to PPM
    if (SaveFrameToPPM(filename, width, height)) {
        std::cout << "[MantleRenderer] Frame dumped: " << filename << std::endl;
        frame_dump_count_++;
    } else {
        std::cout << "[MantleRenderer] Failed to dump frame: " << filename << std::endl;
    }
}

bool VulkanRenderer::SaveFrameToPPM(const char* filename, uint32_t width, uint32_t height) {
    // Ensure output directory exists without shelling out.
    std::error_code ec;
    std::filesystem::create_directories("output", ec);

    // Open file for binary writing
    FILE* file = fopen(filename, "wb");
    if (!file) {
        SetError("Failed to open file for writing: %s", filename);
        return false;
    }
    
    // Write PPM header (P6 = binary RGB format)
    // Format: P6\nwidth height\n255\npixel_data
    fprintf(file, "P6\n%u %u\n255\n", width, height);
    
    // Write pixel data (RGB triplets)
    // Convert from RGBA to RGB by skipping the alpha channel
    const auto pixel_count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    for (std::size_t i = 0; i < pixel_count; ++i) {
        uint32_t rgba = pixel_buffer_[i];

        // Extract RGBA components (assuming little-endian: 0xAABBGGRR)
        uint8_t r = (rgba >> 0) & 0xFF;
        uint8_t g = (rgba >> 8) & 0xFF;
        uint8_t b = (rgba >> 16) & 0xFF;
        
        // Write RGB triplet
        fputc(r, file);
        fputc(g, file);
        fputc(b, file);
    }
    
    fclose(file);
    return true;
}

// ============================================================================
// Phase 5g: GPU Buffer Management (Implementation)
// ============================================================================

uint32_t VulkanRenderer::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) {
    // Phase 5g - Find suitable memory type for buffer allocation
    
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);
    
    // Search through available memory types
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && 
            (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    
    // Fallback: if exact match not found, return first supported type
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
        if (type_filter & (1 << i)) {
            SetError("WARNING: Memory type %u doesn't have requested properties", i);
            return i;
        }
    }
    
    SetError("No suitable memory type found for allocation");
    return 0;
}

// ============================================================================
// Phase 5f: GPU Buffer Management (Stubs for Phase 5g Implementation)
// ============================================================================

bool VulkanRenderer::CreateQuadGeometry() {
    // Phase 5g - Create vertex and index buffers for quad geometry
    
    // Define quad vertices (normalized square: -0.5 to +0.5)
    // Each vertex: position (2x float) + UV coords (2x float) = 16 bytes
    struct Vertex {
        float pos_x, pos_y;    // Position
        float uv_x, uv_y;      // Texture coordinates
    };
    
    Vertex vertices[] = {
        // Bottom-left
        {-0.5f, -0.5f,  0.0f, 0.0f},
        // Top-left
        {-0.5f,  0.5f,  0.0f, 1.0f},
        // Top-right
        { 0.5f,  0.5f,  1.0f, 1.0f},
        // Bottom-right
        { 0.5f, -0.5f,  1.0f, 0.0f},
    };
    
    uint32_t indices[] = {
        // First triangle
        0, 1, 2,
        // Second triangle
        0, 2, 3,
    };
    
    // Create vertex buffer
    if (!CreateBuffer(
        sizeof(vertices),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        quad_vertex_buffer_,
        quad_vertex_memory_
    )) {
        SetError("Failed to create quad vertex buffer");
        return false;
    }
    
    // Copy vertex data to GPU
    void* vertex_data;
    if (vkMapMemory(device_, quad_vertex_memory_, 0, sizeof(vertices), 0, &vertex_data) == VK_SUCCESS) {
        memcpy(vertex_data, vertices, sizeof(vertices));
        vkUnmapMemory(device_, quad_vertex_memory_);
    } else {
        SetError("Failed to map quad vertex buffer memory");
        DestroyBuffer(quad_vertex_buffer_, quad_vertex_memory_);
        return false;
    }
    
    // Create index buffer
    if (!CreateBuffer(
        sizeof(indices),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        quad_index_buffer_,
        quad_index_memory_
    )) {
        SetError("Failed to create quad index buffer");
        DestroyBuffer(quad_vertex_buffer_, quad_vertex_memory_);
        return false;
    }
    
    // Copy index data to GPU
    void* index_data;
    if (vkMapMemory(device_, quad_index_memory_, 0, sizeof(indices), 0, &index_data) == VK_SUCCESS) {
        memcpy(index_data, indices, sizeof(indices));
        vkUnmapMemory(device_, quad_index_memory_);
    } else {
        SetError("Failed to map quad index buffer memory");
        DestroyBuffer(quad_vertex_buffer_, quad_vertex_memory_);
        DestroyBuffer(quad_index_buffer_, quad_index_memory_);
        return false;
    }
    
    std::cout << "[MantleRenderer] Phase 5g: CreateQuadGeometry() - quad vertex buffer (" 
              << sizeof(vertices) << " bytes) + index buffer (" << sizeof(indices) << " bytes)" << std::endl;
    return true;
}

bool VulkanRenderer::CreateInstanceBuffer() {
    // Phase 5g - Create dynamic instance data buffer for sprite rendering
    
    // Define maximum sprites that can be rendered in a single frame
    const uint32_t MAX_SPRITES = 4096;  // Adjust based on performance needs
    
    // Instance data layout per sprite:
    // - TransformPacket (28 bytes): position, rotation, scale, z_index
    // - SpritePacket (32 bytes): texture_id, width, height, color, UV bounds
    // Total: 60 bytes per sprite
    // (We store this compact for efficient GPU transfer)
    
    uint32_t instance_data_size = MAX_SPRITES * (sizeof(TransformPacket) + sizeof(SpritePacket));
    
    // Create instance buffer with GPU-local memory (not host-visible for performance)
    // Uses a staging buffer approach: write to staging, copy to device-local
    if (!CreateBuffer(
        instance_data_size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,  // GPU-local for optimal performance
        instance_buffer_,
        instance_buffer_memory_
    )) {
        SetError("Failed to create instance buffer (size: %u bytes, max sprites: %u)",
                 instance_data_size, MAX_SPRITES);
        return false;
    }
    
    std::cout << "[MantleRenderer] Phase 5g: CreateInstanceBuffer() - instance data buffer (" 
              << instance_data_size << " bytes for " << MAX_SPRITES << " sprites)" << std::endl;
    return true;
}

bool VulkanRenderer::CreateStagingInstanceBuffer() {
    // Phase 5h - Create host-accessible staging buffer for updating instance data
    // This buffer allows us to map memory from CPU and update sprite data
    // before copying to the device-local instance buffer
    
    const uint32_t MAX_SPRITES = 4096;
    uint32_t instance_data_size = MAX_SPRITES * (sizeof(TransformPacket) + sizeof(SpritePacket));
    
    // Create buffer with host-visible and coherent memory for easy CPU updates
    if (!CreateBuffer(
        instance_data_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,  // Used as source for copy operations
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_instance_buffer_,
        staging_instance_memory_
    )) {
        SetError("Failed to create staging instance buffer (size: %u bytes)", instance_data_size);
        return false;
    }
    
    std::cout << "[MantleRenderer] Phase 5h: CreateStagingInstanceBuffer() - host-accessible staging buffer (" 
              << instance_data_size << " bytes)" << std::endl;
    return true;
}

bool VulkanRenderer::CreateDescriptorSets() {
    // Phase 5g - Create descriptor set layout and pool for texture binding
    
    // Define descriptor set layout: one sampler2D binding
    VkDescriptorSetLayoutBinding sampler_layout_binding = {};
    sampler_layout_binding.binding = 0;
    sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.descriptorCount = 1;
    sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    sampler_layout_binding.pImmutableSamplers = nullptr;
    
    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = 1;
    layout_info.pBindings = &sampler_layout_binding;
    
    if (vkCreateDescriptorSetLayout(device_, &layout_info, nullptr, &descriptor_set_layout_) != VK_SUCCESS) {
        SetError("Failed to create descriptor set layout");
        return false;
    }
    
    // Create descriptor pool to allocate descriptor sets
    const uint32_t MAX_DESCRIPTOR_SETS = 256;  // Support up to 256 textures
    
    VkDescriptorPoolSize pool_size = {};
    pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_size.descriptorCount = MAX_DESCRIPTOR_SETS;
    
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    pool_info.maxSets = MAX_DESCRIPTOR_SETS;
    
    if (vkCreateDescriptorPool(device_, &pool_info, nullptr, &descriptor_pool_) != VK_SUCCESS) {
        SetError("Failed to create descriptor pool");
        vkDestroyDescriptorSetLayout(device_, descriptor_set_layout_, nullptr);
        return false;
    }
    
    // Pre-allocate one descriptor set for placeholder texture
    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool_;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &descriptor_set_layout_;
    
    VkDescriptorSet placeholder_desc_set;
    if (vkAllocateDescriptorSets(device_, &alloc_info, &placeholder_desc_set) != VK_SUCCESS) {
        SetError("Failed to allocate descriptor set for placeholder texture");
        vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
        vkDestroyDescriptorSetLayout(device_, descriptor_set_layout_, nullptr);
        return false;
    }
    
    descriptor_sets_.push_back(placeholder_desc_set);
    
    std::cout << "[MantleRenderer] Phase 5g: CreateDescriptorSets() - layout + pool (max " 
              << MAX_DESCRIPTOR_SETS << " textures)" << std::endl;
    return true;
}

bool VulkanRenderer::CreatePlaceholderTexture() {
    // Phase 5g - Create white placeholder texture (1x1 pixel)
    
    const uint32_t texture_width = 1;
    const uint32_t texture_height = 1;
    const uint32_t white_pixel = 0xFFFFFFFF;  // RGBA: white opaque
    
    // Create staging buffer for pixel data upload
    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;
    
    if (!CreateBuffer(
        sizeof(white_pixel),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer,
        staging_memory
    )) {
        SetError("Failed to create staging buffer for placeholder texture");
        return false;
    }
    
    // Copy pixel data to staging buffer
    void* staging_data;
    if (vkMapMemory(device_, staging_memory, 0, sizeof(white_pixel), 0, &staging_data) == VK_SUCCESS) {
        memcpy(staging_data, &white_pixel, sizeof(white_pixel));
        vkUnmapMemory(device_, staging_memory);
    } else {
        SetError("Failed to map staging buffer for placeholder texture");
        DestroyBuffer(staging_buffer, staging_memory);
        return false;
    }
    
    // Create VkImage (GPU texture)
    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_info.extent.width = texture_width;
    image_info.extent.height = texture_height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    
    if (vkCreateImage(device_, &image_info, nullptr, &placeholder_texture_) != VK_SUCCESS) {
        SetError("Failed to create placeholder texture image");
        DestroyBuffer(staging_buffer, staging_memory);
        return false;
    }
    
    // Allocate GPU memory for image
    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(device_, placeholder_texture_, &mem_requirements);
    
    uint32_t memory_type = FindMemoryType(mem_requirements.memoryTypeBits,
                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = memory_type;
    
    if (vkAllocateMemory(device_, &alloc_info, nullptr, &placeholder_texture_memory_) != VK_SUCCESS) {
        SetError("Failed to allocate GPU memory for placeholder texture");
        vkDestroyImage(device_, placeholder_texture_, nullptr);
        DestroyBuffer(staging_buffer, staging_memory);
        return false;
    }
    
    if (vkBindImageMemory(device_, placeholder_texture_, placeholder_texture_memory_, 0) != VK_SUCCESS) {
        SetError("Failed to bind placeholder texture memory");
        vkFreeMemory(device_, placeholder_texture_memory_, nullptr);
        vkDestroyImage(device_, placeholder_texture_, nullptr);
        DestroyBuffer(staging_buffer, staging_memory);
        return false;
    }
    
    // Copy staging buffer to image (via command buffer)
    // Note: vkCmdCopyBufferToImage would require command buffer submission
    // For now: store placeholder_texture for later use (Phase 5h)
    // In production, this would transition image layout and copy data
    
    // Create image view for shader access
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = placeholder_texture_;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    
    if (vkCreateImageView(device_, &view_info, nullptr, &placeholder_texture_view_) != VK_SUCCESS) {
        SetError("Failed to create placeholder texture image view");
        vkFreeMemory(device_, placeholder_texture_memory_, nullptr);
        vkDestroyImage(device_, placeholder_texture_, nullptr);
        DestroyBuffer(staging_buffer, staging_memory);
        return false;
    }
    
    // Clean up staging buffer
    DestroyBuffer(staging_buffer, staging_memory);
    
    std::cout << "[MantleRenderer] Phase 5g: CreatePlaceholderTexture() - 1x1 white texture created" << std::endl;
    return true;
}

bool VulkanRenderer::CreateTextureSampler() {
    // Phase 5g - Create VkSampler for texture filtering and addressing
    
    VkSamplerCreateInfo sampler_info = {};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    
    // Linear filtering for smooth texture interpolation
    sampler_info.magFilter = VK_FILTER_LINEAR;  // Magnification filter
    sampler_info.minFilter = VK_FILTER_LINEAR;  // Minification filter
    
    // Address modes: CLAMP_TO_EDGE prevents texture bleeding at boundaries
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    
    // Anisotropic filtering (optional, requires feature check)
    sampler_info.anisotropyEnable = VK_FALSE;
    sampler_info.maxAnisotropy = 1.0f;
    
    // Comparison settings (for shadow maps, not used here)
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    
    // Mip levels
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;  // No mipmaps for simplicity
    
    if (vkCreateSampler(device_, &sampler_info, nullptr, &texture_sampler_) != VK_SUCCESS) {
        SetError("Failed to create texture sampler");
        return false;
    }
    
    std::cout << "[MantleRenderer] Phase 5g: CreateTextureSampler() - linear filtering texture sampler" << std::endl;
    return true;
}

bool VulkanRenderer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                  VkMemoryPropertyFlags properties,
                                  VkBuffer& buffer, VkDeviceMemory& memory) {
    // Phase 5g - Create and allocate a GPU buffer
    
    buffer = nullptr;
    memory = nullptr;
    
    // Create VkBuffer
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (vkCreateBuffer(device_, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
        SetError("Failed to create VkBuffer (size: %zu bytes)", size);
        return false;
    }
    
    // Query memory requirements
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device_, buffer, &mem_requirements);
    
    // Find suitable memory type
    uint32_t memory_type = FindMemoryType(mem_requirements.memoryTypeBits, properties);
    
    // Allocate GPU memory
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = memory_type;
    
    if (vkAllocateMemory(device_, &alloc_info, nullptr, &memory) != VK_SUCCESS) {
        SetError("Failed to allocate GPU memory (size: %zu bytes)", mem_requirements.size);
        vkDestroyBuffer(device_, buffer, nullptr);
        buffer = nullptr;
        return false;
    }
    
    // Bind memory to buffer
    if (vkBindBufferMemory(device_, buffer, memory, 0) != VK_SUCCESS) {
        SetError("Failed to bind buffer memory");
        vkFreeMemory(device_, memory, nullptr);
        vkDestroyBuffer(device_, buffer, nullptr);
        memory = nullptr;
        buffer = nullptr;
        return false;
    }
    
    return true;
}

void VulkanRenderer::CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
    // Phase 5g - Copy data between GPU buffers
    // Uses command buffer to record and execute copy operation

    // Allocate temporary command buffer for transfer
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = command_pool_;
    alloc_info.commandBufferCount = 1;
    
    VkCommandBuffer cmd_buffer;
    if (vkAllocateCommandBuffers(device_, &alloc_info, (void**)&cmd_buffer) != VK_SUCCESS) {
        SetError("Failed to allocate temporary command buffer for CopyBuffer");
        return;
    }
    
    // Begin recording commands
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    vkBeginCommandBuffer(cmd_buffer, &begin_info);
    
    // Record copy command
    VkBufferCopy copy_region = {};
    copy_region.size = size;
    vkCmdCopyBuffer(cmd_buffer, src_buffer, dst_buffer, 1, &copy_region);
    
    // End recording
    vkEndCommandBuffer(cmd_buffer);
    
    // Submit to graphics queue
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = (const void* const*)&cmd_buffer;
    
    if (vkQueueSubmit(graphics_queue_, 1, &submit_info, nullptr) != VK_SUCCESS) {
        SetError("Failed to submit copy buffer command");
        vkFreeCommandBuffers(device_, command_pool_, 1, (const void* const*)&cmd_buffer);
        return;
    }
    
    // Wait for completion
    vkQueueWaitIdle(graphics_queue_);
    
    // Free temporary command buffer
    vkFreeCommandBuffers(device_, command_pool_, 1, (const void* const*)&cmd_buffer);
}

void VulkanRenderer::DestroyBuffer(VkBuffer& buffer, VkDeviceMemory& memory) {
    // Phase 5g - Cleanup GPU buffer and memory (safety-checked)
    
    if (buffer != nullptr) {
        vkDestroyBuffer(device_, buffer, nullptr);
        buffer = nullptr;
    }
    
    if (memory != nullptr) {
        vkFreeMemory(device_, memory, nullptr);
        memory = nullptr;
    }
}

void VulkanRenderer::UpdateInstanceData(uint32_t sprite_index, const SpritePacket& sprite,
                                        const TransformPacket& transform, uint8_t* data_ptr) {
    // Phase 5g - Copy per-sprite transform and sprite data to GPU instance buffer
    // Instance data layout (60 bytes per sprite):
    //   [0-27]:   TransformPacket (28 bytes)
    //   [28-59]:  SpritePacket (32 bytes)
    
    if (!data_ptr) {
        return;  // Safety check
    }
    
    // Calculate offset for this sprite in the instance data buffer
    uint32_t bytes_per_sprite = sizeof(TransformPacket) + sizeof(SpritePacket);
    uint32_t sprite_offset = sprite_index * bytes_per_sprite;
    
    // Copy transform data
    uint8_t* transform_ptr = data_ptr + sprite_offset;
    memcpy(transform_ptr, &transform, sizeof(TransformPacket));
    
    // Copy sprite data
    uint8_t* sprite_ptr = data_ptr + sprite_offset + sizeof(TransformPacket);
    memcpy(sprite_ptr, &sprite, sizeof(SpritePacket));
}
