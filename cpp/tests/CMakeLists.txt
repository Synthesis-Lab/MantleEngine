cmake_minimum_required(VERSION 3.25)
project(MantleRendererTests)

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# Vulkan Test executable
add_executable(test_vulkan_renderer
    test_vulkan_renderer.cpp
)

target_link_libraries(test_vulkan_renderer
    PRIVATE
        mantle_renderer
        gtest
        gtest_main
)

# Register Vulkan test
add_test(NAME VulkanRendererTests COMMAND test_vulkan_renderer)

# OpenGL Test executable
add_executable(test_opengl_renderer
    test_opengl_renderer.cpp
)

target_link_libraries(test_opengl_renderer
    PRIVATE
        mantle_renderer
        gtest
        gtest_main
)

# Register OpenGL test
add_test(NAME OpenGLRendererTests COMMAND test_opengl_renderer)

# Visual Comparison executable (Phase 5b+5c demo)
add_executable(visual_comparison
    visual_comparison.cpp
)

target_link_libraries(visual_comparison
    PRIVATE
        mantle_renderer
)

# This is a demo program, not a test
# Run with: ./visual_comparison
