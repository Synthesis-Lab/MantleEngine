cmake_minimum_required(VERSION 3.25)
project(MantleRenderer VERSION 0.4.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Vulkan dependency
find_package(Vulkan)

if(NOT Vulkan_FOUND)
    message(STATUS "Vulkan SDK not found, using stub headers from cpp/vulkan/")
    # Use stub Vulkan headers from local directory
    set(Vulkan_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/vulkan")
    set(Vulkan_LIBRARIES "")
endif()

# OpenGL dependency (for OpenGL backend)
find_package(OpenGL QUIET)
if(NOT OpenGL_FOUND)
    message(STATUS "OpenGL not found, OpenGL backend will be stub-only")
    set(USE_OPENGL OFF)
else()
    set(USE_OPENGL ON)
    message(STATUS "OpenGL found, OpenGL backend enabled")
endif()

# GLFW3 dependency (for window creation)
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    message(STATUS "GLFW3 not found, OpenGL backend limited to headless mode")
    set(USE_GLFW3 OFF)
else()
    set(USE_GLFW3 ON)
    message(STATUS "GLFW3 found, OpenGL windowed mode enabled")
endif()

# Core renderer library
add_library(mantle_renderer
    vulkan_renderer.cpp
    opengl/opengl_renderer.cpp
    renderer.cpp
    vulkan/vulkan_stub.cpp
)

target_include_directories(mantle_renderer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(mantle_renderer
    PUBLIC
        ${Vulkan_LIBRARIES}
)

# Add OpenGL if available
if(USE_OPENGL)
    target_include_directories(mantle_renderer PUBLIC ${OPENGL_INCLUDE_DIR})
    target_link_libraries(mantle_renderer PUBLIC ${OPENGL_LIBRARIES})
    target_compile_definitions(mantle_renderer PUBLIC USE_OPENGL=1)
endif()

# Add GLFW3 if available
if(USE_GLFW3)
    target_link_libraries(mantle_renderer PUBLIC glfw)
    target_compile_definitions(mantle_renderer PUBLIC USE_GLFW3=1)
endif()

# Compiler flags
if(MSVC)
    target_compile_options(mantle_renderer PRIVATE /W4)
else()
    target_compile_options(mantle_renderer PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Export library for linking with Rust
set_target_properties(mantle_renderer PROPERTIES
    PUBLIC_HEADER "include/renderer.h;include/vulkan_renderer.h;include/opengl_renderer.h;include/render_packet.h"
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
)

# Installation (optional, for development)
install(TARGETS mantle_renderer
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/mantle
)
