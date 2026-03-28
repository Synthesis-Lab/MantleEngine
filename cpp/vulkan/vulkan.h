#ifndef VULKAN_H_
#define VULKAN_H_

// Stub Vulkan header for development without full SDK
// This allows compilation when Vulkan SDK is not installed

#include <cstdint>
#include <cstddef>
#include "vulkan_struct.h"

// Result enum
typedef enum {
    VK_SUCCESS = 0,
    VK_NOT_READY = 1,
    VK_TIMEOUT = 2,
    VK_EVENT_SET = 3,
    VK_EVENT_RESET = 4,
    VK_INCOMPLETE = 5,
    VK_ERROR_DEVICE_LOST = -1,
    VK_ERROR_EXTENSION_NOT_PRESENT = -7,
    VK_ERROR_FEATURE_NOT_PRESENT = -8,
    VK_ERROR_INCOMPATIBLE_DRIVER = -9,
    VK_ERROR_TOO_MANY_OBJECTS = -10,
    VK_ERROR_FORMAT_NOT_SUPPORTED = -11,
} VkResult;

// Version macros
#define VK_API_VERSION_1_2 VK_MAKE_VERSION(1, 2, 0)
#define VK_MAKE_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))

// Vulkan handle typedefs (opaque pointers)
typedef struct VkInstance_T* VkInstance;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkDevice_T* VkDevice;
typedef struct VkQueue_T* VkQueue;
typedef struct VkCommandPool_T* VkCommandPool;
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VkRenderPass_T* VkRenderPass;
typedef struct VkFramebuffer_T* VkFramebuffer;
typedef struct VkSemaphore_T* VkSemaphore;
typedef struct VkFence_T* VkFence;
typedef struct VkPipeline_T* VkPipeline;

// Basic types
typedef uint32_t VkFlags;
typedef uint32_t VkBool32;
typedef uint32_t VkDeviceSize;
typedef uint32_t VkFormat;

// Extent for images
typedef struct {
    uint32_t width;
    uint32_t height;
} VkExtent2D;

// Stub function declarations
VkResult vkCreateInstance(const void* pCreateInfo, const void* pAllocator, VkInstance* pInstance);
void vkDestroyInstance(VkInstance instance, const void* pAllocator);
VkResult vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices);
void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, void* pProperties);
void vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, void* pQueueFamilyProperties);
VkResult vkCreateDevice(VkPhysicalDevice physicalDevice, const void* pCreateInfo, const void* pAllocator, VkDevice* pDevice);
void vkDestroyDevice(VkDevice device, const void* pAllocator);
void vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue);
VkResult vkCreateCommandPool(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkCommandPool* pCommandPool);
void vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const void* pAllocator);
VkResult vkDeviceWaitIdle(VkDevice device);
VkResult vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const void* pAllocator);
VkResult vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const void* pAllocator);
VkResult vkDestroyImageView(VkDevice device, VkImageView imageView, const void* pAllocator);
VkResult vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const void* pAllocator);
VkResult vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const void* pAllocator);

#endif // VULKAN_H_
