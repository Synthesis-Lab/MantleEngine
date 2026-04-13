cmake_minimum_required(VERSION 3.25)
project(MantleRendererTests)

set(CMAKE_WARN_DEPRECATED OFF)

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

option(MANTLE_ENABLE_OPENGL_TESTS "Enable OpenGL renderer tests" OFF)

if(MANTLE_ENABLE_OPENGL_TESTS)
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
else()
    message(STATUS "OpenGLRendererTests disabled (no DISPLAY detected)")
endif()

# Runtime Test executable
add_executable(test_runtime
    test_runtime.cpp
)

target_link_libraries(test_runtime
    PRIVATE
        mantle_renderer
        gtest
        gtest_main
)

add_test(NAME RuntimeTests COMMAND test_runtime)

# All Engines Integration Test
add_executable(test_all_engines_integration
    test_all_engines_integration.cpp
)

target_link_libraries(test_all_engines_integration
    PRIVATE
        mantle_renderer
        gtest
        gtest_main
)

add_test(NAME AllEnginesIntegrationTests COMMAND test_all_engines_integration)

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
