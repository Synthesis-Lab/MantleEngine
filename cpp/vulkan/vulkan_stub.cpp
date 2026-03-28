// Stub Vulkan function implementations for development without SDK
// These are minimal stubs that allow linking without the actual Vulkan library

#include "vulkan/vulkan.h"
#include <cstring>

// Instance functions
VkResult vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, 
                         const void* pAllocator, 
                         VkInstance* pInstance) {
    if (pInstance) {
        *pInstance = reinterpret_cast<VkInstance>(new char[256]); // Dummy allocation
    }
    return VK_SUCCESS;
}

void vkDestroyInstance(VkInstance instance, const void* pAllocator) {
    if (instance) {
        delete[] reinterpret_cast<char*>(instance);
    }
}

// Physical device enumeration
VkResult vkEnumeratePhysicalDevices(VkInstance instance, 
                                    uint32_t* pPhysicalDeviceCount, 
                                    VkPhysicalDevice* pPhysicalDevices) {
    if (!pPhysicalDeviceCount) return VK_SUCCESS;
    
    if (!pPhysicalDevices) {
        *pPhysicalDeviceCount = 1; // Report 1 device available
        return VK_SUCCESS;
    }
    
    if (*pPhysicalDeviceCount > 0) {
        pPhysicalDevices[0] = reinterpret_cast<VkPhysicalDevice>(new char[256]);
    }
    *pPhysicalDeviceCount = 1;
    return VK_SUCCESS;
}

// Physical device properties
void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, 
                                   VkPhysicalDeviceProperties* pProperties) {
    if (pProperties) {
        memset(pProperties, 0, sizeof(VkPhysicalDeviceProperties));
        pProperties->apiVersion = VK_API_VERSION_1_2;
        strncpy(pProperties->deviceName, "Stub Vulkan Device", 255);
        pProperties->deviceName[255] = '\0';
    }
}

// Queue family properties
void vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, 
                                              uint32_t* pQueueFamilyPropertyCount, 
                                              VkQueueFamilyProperties* pQueueFamilyProperties) {
    if (!pQueueFamilyPropertyCount) return;
    
    if (!pQueueFamilyProperties) {
        *pQueueFamilyPropertyCount = 1;
        return;
    }
    
    if (*pQueueFamilyPropertyCount > 0) {
        pQueueFamilyProperties[0].queueFlags = VK_QUEUE_GRAPHICS_BIT;
        pQueueFamilyProperties[0].queueCount = 1;
    }
    *pQueueFamilyPropertyCount = 1;
}

// Device creation
VkResult vkCreateDevice(VkPhysicalDevice physicalDevice, 
                       const VkDeviceCreateInfo* pCreateInfo, 
                       const void* pAllocator, 
                       VkDevice* pDevice) {
    if (pDevice) {
        *pDevice = reinterpret_cast<VkDevice>(new char[256]);
    }
    return VK_SUCCESS;
}

void vkDestroyDevice(VkDevice device, const void* pAllocator) {
    if (device) {
        delete[] reinterpret_cast<char*>(device);
    }
}

// Queue operations
void vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, 
                     uint32_t queueIndex, VkQueue* pQueue) {
    if (pQueue) {
        *pQueue = reinterpret_cast<VkQueue>(new char[256]);
    }
}

VkResult vkDeviceWaitIdle(VkDevice device) {
    return VK_SUCCESS;
}

// Command pool
VkResult vkCreateCommandPool(VkDevice device, 
                            const VkCommandPoolCreateInfo* pCreateInfo, 
                            const void* pAllocator, 
                            VkCommandPool* pCommandPool) {
    if (pCommandPool) {
        *pCommandPool = reinterpret_cast<VkCommandPool>(new char[256]);
    }
    return VK_SUCCESS;
}

void vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, 
                         const void* pAllocator) {
    if (commandPool) {
        delete[] reinterpret_cast<char*>(commandPool);
    }
}

// Surface and swapchain
VkResult vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, 
                            const void* pAllocator) {
    if (surface) {
        delete[] reinterpret_cast<char*>(surface);
    }
    return VK_SUCCESS;
}

VkResult vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, 
                              const void* pAllocator) {
    if (swapchain) {
        delete[] reinterpret_cast<char*>(swapchain);
    }
    return VK_SUCCESS;
}

// Image views
VkResult vkDestroyImageView(VkDevice device, VkImageView imageView, 
                           const void* pAllocator) {
    if (imageView) {
        delete[] reinterpret_cast<char*>(imageView);
    }
    return VK_SUCCESS;
}

// Render pass
VkResult vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass, 
                            const void* pAllocator) {
    if (renderPass) {
        delete[] reinterpret_cast<char*>(renderPass);
    }
    return VK_SUCCESS;
}

// Framebuffer
VkResult vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, 
                             const void* pAllocator) {
    if (framebuffer) {
        delete[] reinterpret_cast<char*>(framebuffer);
    }
    return VK_SUCCESS;
}
