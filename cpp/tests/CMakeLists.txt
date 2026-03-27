cmake_minimum_required(VERSION 3.16)
project(MantleRendererTests)

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(test_vulkan_renderer
    test_vulkan_renderer.cpp
)

target_link_libraries(test_vulkan_renderer
    PRIVATE
        mantle_renderer
        gtest
        gtest_main
)

# Register test
add_test(NAME VulkanRendererTests COMMAND test_vulkan_renderer)
