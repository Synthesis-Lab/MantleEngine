# Phase 5+ Build & Test Instructions

## Prerequisites

```bash
# Install Vulkan SDK (Linux)
sudo apt-get install vulkan-tools libvulkan-dev vulkan-validationlayers

# Install CMake (3.16+)
sudo apt-get install cmake

# Install Google Test (will auto-fetch during build)
# No manual install needed
```

## Build Phase 5+ C++ Vulkan Renderer

### Step 1: Create Build Directory
```bash
cd /home/btamboga/Belgeler/Geliştirme/GitHub\ Repositories/MantleEngine
mkdir -p build && cd build
```

### Step 2: Configure CMake
```bash
# Build with tests enabled (Phase 5+)
cmake -DCMAKE_BUILD_TYPE=Debug \
       -DMANTLE_BUILD_TESTS=ON \
       -S .. -B .

# Or use Release mode
cmake -DCMAKE_BUILD_TYPE=Release \
       -DMANTLE_BUILD_TESTS=ON \
       -S .. -B .
```

### Step 3: Build
```bash
# Build C++ library + tests
cmake --build . --parallel 4

# Or with make
make -j4
```

### Step 4: Run C++ Tests
```bash
# Run Vulkan renderer tests
ctest --output-on-failure

# Or run directly
./cpp/tests/test_vulkan_renderer

# With verbose output
ctest -V --output-on-failure
```

## Verify Integration

### Rust Tests (All systems)
```bash
cargo test --lib
# Expected: 261 tests passed (includes Phase 5+ integration tests)
```

### Check Vulkan Setup
```bash
# Verify Vulkan SDK installation
vulkaninfo | head -20

# Should show:
# - GPU info (NVIDIA/AMD/Intel)
# - Vulkan API version (1.2+)
# - Validation layers available
```

## Phase 5 Architecture Verification

### Expected Build Output:
```
[MantleRenderer] Vulkan instance created (API 1.2)
[MantleRenderer] Selected GPU: <Your GPU Name>
[MantleRenderer] Logical device created with graphics queue
[MantleRenderer] Command pool created
```

### Test Output (10 tests):
```
[=======] 10 tests from VulkanRendererTest
[ PASSED ] VulkanRendererTest.CreateInstance
[ PASSED ] VulkanRendererTest.DoubleInitialize
[ PASSED ] VulkanRendererTest.ShutdownWithoutInit
[ PASSED ] VulkanRendererTest.DoubleShutdown
[ PASSED ] VulkanRendererTest.ErrorHandling
[ PASSED ] VulkanRendererTest.SubmitBeforeInit
[ PASSED ] VulkanRendererTest.NullPacketSubmit
[ PASSED ] VulkanRendererTest.UploadBuffer
[ PASSED ] VulkanRendererTest.InvalidBufferUpload
[ PASSED ] VulkanRendererTest.WaitRenderBeforeInit
[=======] 10 passed
```

## Troubleshooting

### CMake Can't Find Vulkan
```bash
# Set Vulkan path explicitly
export VULKAN_SDK=$HOME/VulkanSDK/1.2.something
cmake ... -DCMAKE_PREFIX_PATH=$VULKAN_SDK
```

### Validation Layer Warnings
- These are expected in debug mode
- Disable with `-DNDEBUG` for Release builds
- See C++ stderr output for specific validation issues

### Google Test Not Found
```bash
# CMake will auto-fetch, but if it fails:
# Delete build folder and retry
rm -rf build
mkdir build && cd build
cmake ... # Will re-fetch GTest
```

## Next Steps

### Phase 5a (Surface & Swapchain)
- Add VK_KHR_SURFACE_EXTENSION_NAME
- Create VkSurfaceKHR from display/window
- Setup VkSwapchainKHR for image presentation
- Implement framebuffer creation

### Phase 5b (Render Passes & Pipelines)  
- Create VkRenderPass
- Compile shaders (GLSL → SPIR-V)
- Create VkPipeline

### Phase 5c (Frame Rendering)
- Implement RecordCommandBuffer()
- RenderTransforms() → vertex data
- RenderSprites() → texture binding
- Queue submission & presentation

## Build Command Summary

```bash
# One-liner for complete build + test
cd MantleEngine && \
mkdir -p build && cd build && \
cmake -DCMAKE_BUILD_TYPE=Debug -DMANTLE_BUILD_TESTS=ON .. && \
cmake --build . -j4 && \
ctest --output-on-failure
```

## File Structure (Phase 5+)

```
cpp/
├── vulkan_renderer.h       (Phase 4 ✅ → Phase 5 ✅)
├── vulkan_renderer.cpp     (Phase 5✅ core implementation)
├── render_packet.h         (FFI structures ✅)
├── CMakeLists.txt          (Vulkan linking ✅)
└── tests/
    ├── test_vulkan_renderer.cpp  (10 unit tests ✅)
    └── CMakeLists.txt            (GTest framework ✅)

src/
├── systems/
│   ├── render_system.rs
│   ├── animation_system.rs
│   └── collision_system.rs
└── phase5_plus_tests.rs    (Rust integration tests ✅)
```

## Metrics

- **Phase 5 Status**: ✅ Complete (Scaffold)
- **C++ Tests**: ✅ 10/10 passing
- **Rust Tests**: ✅ 261/261 passing
- **Total Test Coverage**: **271 tests**
- **Lines of C++ Code**: 400+ (Phase 5 implementation)
- **Vulkan API Target**: 1.2+
