#include "vulkan_renderer.h"
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <vector>

// Phase 5+: Include Vulkan headers for actual implementation
#include <vulkan/vulkan.h>

// ============================================================================
// VulkanRenderer - Phase 4 (Skeleton) → Phase 5+ (Full Implementation)
// ============================================================================
// 
// Architecture Overview:
// 
//   Game Thread (Rust)              Render Thread (C++)
//   ───────────────────              ──────────────────
//   [Scene Tree]                     [VulkanRenderer]
//       ↓                                   ↑
//   [FFI RenderPacket] ────────────────────┤
//       ↓ (submit)
//   [C ABI] ────────────────→ [SubmitRenderPacket()]
//       ↓ (wait)
//       ←──────────────────── [WaitRender()]
//
// Phases:
//   Phase 4: Skeleton - logging, null initialization
//   Phase 5: Core Vulkan setup - instance, device, queues, command pools
//   Phase 5+: Rendering - swapchain, render passes, pipelines, frame recording
//
// Key Structures:
//   - RenderPacket: Frame-scoped data from Rust
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
    
    // Phase 4: Wait for current frame to complete
    // This should synchronize with GPU via vkWaitForFences
    // For skeleton: just increment frame counter
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
    if (!packet) {
        SetError("RenderFrame called with null packet");
        return;
    }
    
    std::cout << "[MantleRenderer] Frame " << packet->frame_number 
              << ": " << packet->transform_count << " transforms, "
              << packet->sprite_count << " sprites, "
              << packet->collider_count << " colliders" << std::endl;
    
    // Phase 5+: Full frame rendering pipeline
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

void VulkanRenderer::RecordCommandBuffer(uint32_t image_index, const RenderPacket* packet) {
    // Phase 5 - Command Buffer Recording
    // Step 1: Begin command buffer recording
    //   - VkCommandBufferBeginInfo with VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    //   - vkBeginCommandBuffer(cmd_buffer_, &begin_info)
    //
    // Step 2: Begin render pass
    //   - VkRenderPassBeginInfo with clear values (color, depth)
    //   - vkCmdBeginRenderPass(cmd_buffer_, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE)
    //
    // Step 3: Bind graphics pipeline
    //   - vkCmdBindPipeline(cmd_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_)
    //
    // Step 4: Set dynamic state
    //   - vkCmdSetViewport(), vkCmdSetScissor()
    //
    // Step 5: Call rendering methods by type
    //   - RenderTransforms(cmd_buffer_, packet)
    //   - RenderSprites(cmd_buffer_, packet)
    //   - RenderColliders(cmd_buffer_, packet)
    //
    // Step 6: End render pass
    //   - vkCmdEndRenderPass(cmd_buffer_)
    //
    // Step 7: End command buffer
    //   - vkEndCommandBuffer(cmd_buffer_)
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

void VulkanRenderer::SetError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(last_error_, sizeof(last_error_), format, args);
    va_end(args);
    
    std::cerr << "[MantleRenderer ERROR] " << last_error_ << std::endl;
}
