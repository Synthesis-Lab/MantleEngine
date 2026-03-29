// Additional Vulkan structures and enums for stub implementation
#ifndef VULKAN_STRUCT_H_
#define VULKAN_STRUCT_H_

#include <cstdint>
#include <cstring>

// Structure types
enum VkStructureType {
    VK_STRUCTURE_TYPE_APPLICATION_INFO = 0,
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO = 1,
    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO = 2,
    VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO = 3,
    VK_STRUCTURE_TYPE_SUBMIT_INFO = 4,
    VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO = 5,
    VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE = 6,
    VK_STRUCTURE_TYPE_BIND_SPARSE_INFO = 7,
    VK_STRUCTURE_TYPE_FENCE_CREATE_INFO = 8,
    VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO = 9,
    VK_STRUCTURE_TYPE_EVENT_CREATE_INFO = 10,
    VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO = 11,
    VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO = 12,
    VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO = 13,
    VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO = 14,
    VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO = 15,
    VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO = 16,
    VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO = 17,
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO = 18,
    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO = 19,
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO = 20,
    VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO = 21,
    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO = 22,
    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO = 23,
    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO = 24,
    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO = 25,
    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO = 26,
    VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO = 27,
    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO = 28,
    VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO = 29,
    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO = 30,
    VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO = 31,
    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO = 32,
    VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO = 33,
    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO = 34,
    VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET = 35,
    VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET = 36,
    VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO = 37,
    VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO = 38,
    VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION = 39,
    VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE = 40,
    VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION = 41,
    VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY = 42,
    VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO = 16,
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO = 50,
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO = 51,
    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO = 53,
};

// Queue flags
enum VkQueueFlagBits {
    VK_QUEUE_GRAPHICS_BIT = 0x00000001,
    VK_QUEUE_COMPUTE_BIT = 0x00000002,
    VK_QUEUE_TRANSFER_BIT = 0x00000004,
};
typedef uint32_t VkQueueFlags;

// Command pool create flags
enum VkCommandPoolCreateFlagBits {
    VK_COMMAND_POOL_CREATE_TRANSIENT_BIT = 0x00000001,
    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT = 0x00000002,
};

// Format enum (Phase 5b)
enum VkFormat {
    VK_FORMAT_UNDEFINED = 0,
    VK_FORMAT_R8_UNORM = 9,
    VK_FORMAT_R8G8B8A8_UNORM = 37,
    VK_FORMAT_B8G8R8A8_UNORM = 44,
};

// Subpass dependency constants (Phase 5b)
#define VK_SUBPASS_EXTERNAL (~0U)

// Application info structure
typedef struct {
    VkStructureType sType;
    const void* pNext;
    const char* pApplicationName;
    uint32_t applicationVersion;
    const char* pEngineName;
    uint32_t engineVersion;
    uint32_t apiVersion;
} VkApplicationInfo;

// Instance create info structure
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    const VkApplicationInfo* pApplicationInfo;
    uint32_t enabledLayerCount;
    const char* const* ppEnabledLayerNames;
    uint32_t enabledExtensionCount;
    const char* const* ppEnabledExtensionNames;
} VkInstanceCreateInfo;

// Device queue create info structure
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t queueFamilyIndex;
    uint32_t queueCount;
    const float* pQueuePriorities;
} VkDeviceQueueCreateInfo;

// Device create info structure
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t queueCreateInfoCount;
    const VkDeviceQueueCreateInfo* pQueueCreateInfos;
    uint32_t enabledLayerCount;
    const char* const* ppEnabledLayerNames;
    uint32_t enabledExtensionCount;
    const char* const* ppEnabledExtensionNames;
    const void* pEnabledFeatures;
} VkDeviceCreateInfo;

// Queue family properties
typedef struct {
    VkQueueFlags queueFlags;
    uint32_t queueCount;
    uint32_t timestampValidBits;
    struct {
        uint32_t width;
        uint32_t height;
        uint32_t depth;
    } minImageTransferGranularity;
} VkQueueFamilyProperties;

// Physical device properties
typedef struct {
    uint32_t apiVersion;
    uint32_t driverVersion;
    uint32_t vendorID;
    uint32_t deviceID;
    uint32_t deviceType;
    char deviceName[256];
    uint8_t pipelineCacheUUID[16];
} VkPhysicalDeviceProperties;

// Command pool create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t queueFamilyIndex;
} VkCommandPoolCreateInfo;

// Physical device features
typedef struct {
    uint32_t robustBufferAccess;
    uint32_t fullDrawIndexUint32;
    uint32_t imageCubeArray;
    uint32_t independentBlend;
    uint32_t geometryShader;
    uint32_t tessellationShader;
    uint32_t sampleRateShading;
    uint32_t dualSrcBlend;
    uint32_t logicOp;
    uint32_t multiDrawIndirect;
    uint32_t drawIndirectFirstInstance;
    uint32_t depthClamp;
    uint32_t depthBiasClamp;
    uint32_t fillModeNonSolid;
    uint32_t depthBounds;
    uint32_t wideLines;
    uint32_t largePoints;
    uint32_t alphaToOne;
    uint32_t multiViewport;
    uint32_t samplerAnisotropy;
    uint32_t textureCompressionETC2;
    uint32_t textureCompressionASTC_LDR;
    uint32_t textureCompressionBC;
    uint32_t occlusionQueryPrecise;
    uint32_t pipelineStatisticsQuery;
    uint32_t vertexPipelineStoresAndAtomics;
    uint32_t fragmentStoresAndAtomics;
    uint32_t shaderTessellationAndGeometryPointSize;
    uint32_t shaderImageGatherExtended;
    uint32_t shaderStorageImageExtendedFormats;
    uint32_t shaderStorageImageMultisample;
    uint32_t shaderStorageImageReadWithoutFormat;
    uint32_t shaderStorageImageWriteWithoutFormat;
    uint32_t shaderUniformBufferArrayDynamicIndexing;
    uint32_t shaderSampledImageArrayDynamicIndexing;
    uint32_t shaderStorageBufferArrayDynamicIndexing;
    uint32_t shaderStorageImageArrayDynamicIndexing;
    uint32_t shaderClipDistance;
    uint32_t shaderCullDistance;
    uint32_t shaderFloat64;
    uint32_t shaderInt64;
    uint32_t shaderInt16;
    uint32_t shaderResourceResidency;
    uint32_t shaderResourceMinLod;
    uint32_t sparseBinding;
    uint32_t sparseResidencyBuffer;
    uint32_t sparseResidencyImage2D;
    uint32_t sparseResidencyImage3D;
    uint32_t sparseResidency2Samples;
    uint32_t sparseResidency4Samples;
    uint32_t sparseResidency8Samples;
    uint32_t sparseResidency16Samples;
    uint32_t sparseResidencyAliased;
    uint32_t variableMultisampleRate;
    uint32_t inheritedQueries;
} VkPhysicalDeviceFeatures;

// Phase 5b: Render Pass and Pipeline structures
enum VkImageLayout {
    VK_IMAGE_LAYOUT_UNDEFINED = 0,
    VK_IMAGE_LAYOUT_GENERAL = 1,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL = 6,
    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL = 7,
    VK_IMAGE_LAYOUT_PREINITIALIZED = 8,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR = 1000001002,
};

enum VkAttachmentLoadOp {
    VK_ATTACHMENT_LOAD_OP_LOAD = 0,
    VK_ATTACHMENT_LOAD_OP_CLEAR = 1,
    VK_ATTACHMENT_LOAD_OP_DONT_CARE = 2,
};

enum VkAttachmentStoreOp {
    VK_ATTACHMENT_STORE_OP_STORE = 0,
    VK_ATTACHMENT_STORE_OP_DONT_CARE = 1,
};

enum VkSampleCountFlagBits {
    VK_SAMPLE_COUNT_1_BIT = 0x00000001,
    VK_SAMPLE_COUNT_2_BIT = 0x00000002,
    VK_SAMPLE_COUNT_4_BIT = 0x00000004,
};

enum VkPipelineBindPoint {
    VK_PIPELINE_BIND_POINT_GRAPHICS = 0,
    VK_PIPELINE_BIND_POINT_COMPUTE = 1,
};

enum VkShaderStageFlagBits {
    VK_SHADER_STAGE_VERTEX_BIT = 0x00000001,
    VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT = 0x00000002,
    VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT = 0x00000004,
    VK_SHADER_STAGE_GEOMETRY_BIT = 0x00000008,
    VK_SHADER_STAGE_FRAGMENT_BIT = 0x00000010,
};

enum VkPrimitiveTopology {
    VK_PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
    VK_PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
};

enum VkPolygonMode {
    VK_POLYGON_MODE_FILL = 0,
    VK_POLYGON_MODE_LINE = 1,
    VK_POLYGON_MODE_POINT = 2,
};

enum VkCullModeFlagBits {
    VK_CULL_MODE_NONE = 0,
    VK_CULL_MODE_FRONT_BIT = 0x00000001,
    VK_CULL_MODE_BACK_BIT = 0x00000002,
};

enum VkFrontFace {
    VK_FRONT_FACE_COUNTER_CLOCKWISE = 0,
    VK_FRONT_FACE_CLOCKWISE = 1,
};

enum VkPipelineStageFlagBits {
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT = 0x00000001,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000080,
};

enum VkAccessFlagBits {
    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT = 0x00000080,
    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT = 0x00000100,
};

enum VkColorComponentFlagBits {
    VK_COLOR_COMPONENT_R_BIT = 0x00000001,
    VK_COLOR_COMPONENT_G_BIT = 0x00000002,
    VK_COLOR_COMPONENT_B_BIT = 0x00000004,
    VK_COLOR_COMPONENT_A_BIT = 0x00000008,
};

// Attachment description
typedef struct {
    uint32_t flags;
    uint32_t format;
    uint32_t samples;
    uint32_t loadOp;
    uint32_t storeOp;
    uint32_t stencilLoadOp;
    uint32_t stencilStoreOp;
    uint32_t initialLayout;
    uint32_t finalLayout;
} VkAttachmentDescription;

// Attachment reference
typedef struct {
    uint32_t attachment;
    uint32_t layout;
} VkAttachmentReference;

// Subpass description
typedef struct {
    uint32_t flags;
    uint32_t pipelineBindPoint;
    uint32_t inputAttachmentCount;
    const VkAttachmentReference* pInputAttachments;
    uint32_t colorAttachmentCount;
    const VkAttachmentReference* pColorAttachments;
    const VkAttachmentReference* pResolveAttachments;
    const VkAttachmentReference* pDepthStencilAttachment;
    uint32_t preserveAttachmentCount;
    const uint32_t* pPreserveAttachments;
} VkSubpassDescription;

// Subpass dependency
typedef struct {
    uint32_t srcSubpass;
    uint32_t dstSubpass;
    uint32_t srcStageMask;
    uint32_t dstStageMask;
    uint32_t srcAccessMask;
    uint32_t dstAccessMask;
    uint32_t dependencyFlags;
} VkSubpassDependency;

// Render pass create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t attachmentCount;
    const VkAttachmentDescription* pAttachments;
    uint32_t subpassCount;
    const VkSubpassDescription* pSubpasses;
    uint32_t dependencyCount;
    const VkSubpassDependency* pDependencies;
} VkRenderPassCreateInfo;

// Shader module create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t codeSize;
    const uint32_t* pCode;
} VkShaderModuleCreateInfo;

// Pipeline shader stage create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t stage;
    void* module; // VkShaderModule
    const char* pName;
    const void* pSpecializationInfo;
} VkPipelineShaderStageCreateInfo;

// Pipeline vertex input state
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t vertexBindingDescriptionCount;
    const void* pVertexBindingDescriptions;
    uint32_t vertexAttributeDescriptionCount;
    const void* pVertexAttributeDescriptions;
} VkPipelineVertexInputStateCreateInfo;

// Pipeline input assembly state
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t topology;
    uint32_t primitiveRestartEnable;
} VkPipelineInputAssemblyStateCreateInfo;

// Viewport
typedef struct {
    float x, y, width, height, minDepth, maxDepth;
} VkViewport;

// Rect2D
typedef struct {
    struct { int32_t x, y; } offset;
    struct { uint32_t width, height; } extent;
} VkRect2D;

// Pipeline viewport state
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t viewportCount;
    const VkViewport* pViewports;
    uint32_t scissorCount;
    const VkRect2D* pScissors;
} VkPipelineViewportStateCreateInfo;

// Pipeline rasterization state
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t depthClampEnable;
    uint32_t rasterizerDiscardEnable;
    uint32_t polygonMode;
    uint32_t cullMode;
    uint32_t frontFace;
    uint32_t depthBiasEnable;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;
    float lineWidth;
} VkPipelineRasterizationStateCreateInfo;

// Pipeline multisample state
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t rasterizationSamples;
    uint32_t sampleShadingEnable;
    float minSampleShading;
    const uint32_t* pSampleMask;
    uint32_t alphaToCoverageEnable;
    uint32_t alphaToOneEnable;
} VkPipelineMultisampleStateCreateInfo;

// Pipeline color blend attachment state
typedef struct {
    uint32_t blendEnable;
    uint32_t srcColorBlendFactor;
    uint32_t dstColorBlendFactor;
    uint32_t colorBlendOp;
    uint32_t srcAlphaBlendFactor;
    uint32_t dstAlphaBlendFactor;
    uint32_t alphaBlendOp;
    uint32_t colorWriteMask;
} VkPipelineColorBlendAttachmentState;

// Pipeline color blend state
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t logicOpEnable;
    uint32_t logicOp;
    uint32_t attachmentCount;
    const VkPipelineColorBlendAttachmentState* pAttachments;
    float blendConstants[4];
} VkPipelineColorBlendStateCreateInfo;

// Pipeline layout create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t setLayoutCount;
    const void* const* pSetLayouts;
    uint32_t pushConstantRangeCount;
    const void* pPushConstantRanges;
} VkPipelineLayoutCreateInfo;

// Pipeline create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t stageCount;
    const VkPipelineShaderStageCreateInfo* pStages;
    const VkPipelineVertexInputStateCreateInfo* pVertexInputState;
    const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
    const void* pTessellationState;
    const VkPipelineViewportStateCreateInfo* pViewportState;
    const VkPipelineRasterizationStateCreateInfo* pRasterizationState;
    const VkPipelineMultisampleStateCreateInfo* pMultisampleState;
    const void* pDepthStencilState;
    const VkPipelineColorBlendStateCreateInfo* pColorBlendState;
    const void* pDynamicState;
    void* layout; // VkPipelineLayout
    void* renderPass; // VkRenderPass
    uint32_t subpass;
    void* basePipelineHandle; // VkPipeline
    int32_t basePipelineIndex;
} VkGraphicsPipelineCreateInfo;

// Phase 5c: Command Buffer Recording & Submission

// Subpass contents (how commands are recorded)
enum VkSubpassContents {
    VK_SUBPASS_CONTENTS_INLINE = 0,
    VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS = 1,
};

// Command buffer usage flags
enum VkCommandBufferUsageFlagBits {
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT = 0x00000001,
    VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT = 0x00000002,
    VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT = 0x00000004,
};

// Command buffer allocate info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    void* commandPool;  // VkCommandPool
    uint32_t level;     // VkCommandBufferLevel
    uint32_t commandBufferCount;
} VkCommandBufferAllocateInfo;

// Command buffer begin info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    const void* pInheritanceInfo;
} VkCommandBufferBeginInfo;

// Clear color value
typedef union {
    float float32[4];
    int32_t int32[4];
    uint32_t uint32[4];
} VkClearColorValue;

// Clear value (color or depth/stencil)
typedef union {
    VkClearColorValue color;
    struct {
        float depth;
        uint32_t stencil;
    } depthStencil;
} VkClearValue;

// Render pass begin info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    void* renderPass; // VkRenderPass
    void* framebuffer; // VkFramebuffer
    VkRect2D renderArea;
    uint32_t clearValueCount;
    const VkClearValue* pClearValues;
} VkRenderPassBeginInfo;

// Fence create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
} VkFenceCreateInfo;

// Note: VkPipelineBindPoint already defined above (line ~249)

// Submit info (for queue submission)
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t waitSemaphoreCount;
    const void* const* pWaitSemaphores;
    const uint32_t* pWaitDstStageMask;
    uint32_t commandBufferCount;
    const void* const* pCommandBuffers;
    uint32_t signalSemaphoreCount;
    const void* const* pSignalSemaphores;
} VkSubmitInfo;

// Phase 5d: Output Integration - Buffer and memory structures
// Memory property flags for buffer allocation
enum VkMemoryPropertyFlagBits {
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT = 0x00000001,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT = 0x00000002,
    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT = 0x00000004,
    VK_MEMORY_PROPERTY_HOST_CACHED_BIT = 0x00000008,
};
typedef uint32_t VkMemoryPropertyFlags;

// Buffer usage flags
enum VkBufferUsageFlagBits {
    VK_BUFFER_USAGE_TRANSFER_DST_BIT = 0x00000001,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT = 0x00000002,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT = 0x00000004,
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT = 0x00000008,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT = 0x00000010,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT = 0x00000020,
};
typedef uint32_t VkBufferUsageFlags;

// Memory allocate info (for GPU memory allocation)
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint64_t allocationSize;
    uint32_t memoryTypeIndex;
} VkMemoryAllocateInfo;

// Buffer create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t createFlags;
    uint64_t size;
    VkBufferUsageFlags usage;
    uint32_t sharingMode;
    uint32_t queueFamilyIndexCount;
    const uint32_t* pQueueFamilyIndices;
} VkBufferCreateInfo;

// Image copy region for buffer→image transfers
typedef struct {
    uint64_t bufferOffset;
    uint32_t bufferRowLength;
    uint32_t bufferImageHeight;
    struct {
        uint32_t x;
        uint32_t y;
        uint32_t z;
    } imageOffset;
    struct {
        uint32_t width;
        uint32_t height;
        uint32_t depth;
    } imageExtent;
} VkBufferImageCopy;

// Phase 5g: Memory structures for GPU allocation
// Memory heap info
typedef struct {
    uint64_t size;
    uint32_t flags;
} VkMemoryHeap;

// Memory type info
typedef struct {
    uint32_t propertyFlags;
    uint32_t heapIndex;
} VkMemoryType;

// Memory requirements for buffer allocation
typedef struct {
    uint64_t size;
    uint64_t alignment;
    uint32_t memoryTypeBits;
} VkMemoryRequirements;

// Physical device memory properties
typedef struct {
    VkMemoryType memoryTypes[32];
    uint32_t memoryTypeCount;
    VkMemoryHeap memoryHeaps[16];
    uint32_t memoryHeapCount;
} VkPhysicalDeviceMemoryProperties;

// Buffer copy region for vkCmdCopyBuffer
typedef struct {
    uint64_t srcOffset;
    uint64_t dstOffset;
    uint64_t size;
} VkBufferCopy;

// Buffer/device memory sharing mode enum
enum VkSharingMode {
    VK_SHARING_MODE_EXCLUSIVE = 0,
    VK_SHARING_MODE_CONCURRENT = 1,
};

// Command buffer level enum
enum VkCommandBufferLevel {
    VK_COMMAND_BUFFER_LEVEL_PRIMARY = 0,
    VK_COMMAND_BUFFER_LEVEL_SECONDARY = 1,
};

// Boolean type and constants
typedef uint32_t VkBool32;
#define VK_TRUE  1
#define VK_FALSE 0

// Filter enum for texture sampling
enum VkFilter {
    VK_FILTER_NEAREST = 0,
    VK_FILTER_LINEAR = 1,
};

// Sampler address mode enum
enum VkSamplerAddressMode {
    VK_SAMPLER_ADDRESS_MODE_REPEAT = 0,
    VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT = 1,
    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE = 2,
    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER = 3,
};

// Sampler mipmap mode enum
enum VkSamplerMipmapMode {
    VK_SAMPLER_MIPMAP_MODE_NEAREST = 0,
    VK_SAMPLER_MIPMAP_MODE_LINEAR = 1,
};

// Compare operation enum
enum VkCompareOp {
    VK_COMPARE_OP_NEVER = 0,
    VK_COMPARE_OP_LESS = 1,
    VK_COMPARE_OP_EQUAL = 2,
    VK_COMPARE_OP_LESS_OR_EQUAL = 3,
    VK_COMPARE_OP_GREATER = 4,
    VK_COMPARE_OP_NOT_EQUAL = 5,
    VK_COMPARE_OP_GREATER_OR_EQUAL = 6,
    VK_COMPARE_OP_ALWAYS = 7,
};

// Image tiling enum
enum VkImageTiling {
    VK_IMAGE_TILING_OPTIMAL = 0,
    VK_IMAGE_TILING_LINEAR = 1,
};

// Image type enum
enum VkImageType {
    VK_IMAGE_TYPE_1D = 0,
    VK_IMAGE_TYPE_2D = 1,
    VK_IMAGE_TYPE_3D = 2,
};

// Image view type enum
enum VkImageViewType {
    VK_IMAGE_VIEW_TYPE_1D = 0,
    VK_IMAGE_VIEW_TYPE_2D = 1,
    VK_IMAGE_VIEW_TYPE_3D = 2,
    VK_IMAGE_VIEW_TYPE_CUBE = 3,
};

// Image aspect flag bits
enum VkImageAspectFlagBits {
    VK_IMAGE_ASPECT_COLOR_BIT = 0x00000001,
    VK_IMAGE_ASPECT_DEPTH_BIT = 0x00000002,
    VK_IMAGE_ASPECT_STENCIL_BIT = 0x00000004,
};
typedef uint32_t VkImageAspectFlags;

// Image usage flag bits
enum VkImageUsageFlagBits {
    VK_IMAGE_USAGE_TRANSFER_DST_BIT = 0x00000002,
    VK_IMAGE_USAGE_SAMPLED_BIT = 0x00000004,
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 0x00000010,
};
typedef uint32_t VkImageUsageFlags;

// Sampler create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    VkFilter magFilter;
    VkFilter minFilter;
    VkSamplerMipmapMode mipmapMode;
    VkSamplerAddressMode addressModeU;
    VkSamplerAddressMode addressModeV;
    VkSamplerAddressMode addressModeW;
    float mipLodBias;
    VkBool32 anisotropyEnable;
    float maxAnisotropy;
    VkBool32 compareEnable;
    VkCompareOp compareOp;
    float minLod;
    float maxLod;
    uint32_t borderColor;
    VkBool32 unnormalizedCoordinates;
} VkSamplerCreateInfo;

// Image extent (size)
typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
} VkExtent3D;

// Image create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    VkImageType imageType;
    VkFormat format;
    VkExtent3D extent;
    uint32_t mipLevels;
    uint32_t arrayLayers;
    uint32_t samples;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    uint32_t sharingMode;
    uint32_t queueFamilyIndexCount;
    const uint32_t* pQueueFamilyIndices;
    uint32_t initialLayout;
} VkImageCreateInfo;

// Image subresource range
typedef struct {
    VkImageAspectFlags aspectMask;
    uint32_t baseMipLevel;
    uint32_t levelCount;
    uint32_t baseArrayLayer;
    uint32_t layerCount;
} VkImageSubresourceRange;

// Image view create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    void* image;  // VkImage
    VkImageViewType viewType;
    VkFormat format;
    uint32_t components;  // VkComponentMapping
    VkImageSubresourceRange subresourceRange;
} VkImageViewCreateInfo;

// Descriptor set layout binding
typedef struct {
    uint32_t binding;
    uint32_t descriptorType;
    uint32_t descriptorCount;
    uint32_t stageFlags;
    const void* pImmutableSamplers;
} VkDescriptorSetLayoutBinding;

// Descriptor set layout create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t bindingCount;
    const VkDescriptorSetLayoutBinding* pBindings;
} VkDescriptorSetLayoutCreateInfo;

// Descriptor pool size
typedef struct {
    uint32_t type;  // VkDescriptorType
    uint32_t descriptorCount;
} VkDescriptorPoolSize;

// Descriptor pool create info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    uint32_t flags;
    uint32_t maxSets;
    uint32_t poolSizeCount;
    const VkDescriptorPoolSize* pPoolSizes;
} VkDescriptorPoolCreateInfo;

// Descriptor set allocate info
typedef struct {
    VkStructureType sType;
    const void* pNext;
    void* descriptorPool;
    uint32_t descriptorSetCount;
    const void* pSetLayouts;  // VkDescriptorSetLayout*
} VkDescriptorSetAllocateInfo;

// Descriptor type enum
enum VkDescriptorType {
    VK_DESCRIPTOR_TYPE_SAMPLER = 0,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
    VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
};

// Index type enum  
enum VkIndexType {
    VK_INDEX_TYPE_UINT16 = 0,
    VK_INDEX_TYPE_UINT32 = 1,
};

// Vulkan structures and definitions end
#endif // VULKAN_STRUCT_H_
