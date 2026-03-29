cmake_minimum_required(VERSION 3.16)
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

# Core renderer library
add_library(mantle_renderer
    vulkan_renderer.cpp
    vulkan/vulkan_stub.cpp
)

target_include_directories(mantle_renderer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(mantle_renderer
    PUBLIC
        ${Vulkan_LIBRARIES}
)

# Compiler flags
if(MSVC)
    target_compile_options(mantle_renderer PRIVATE /W4)
else()
    target_compile_options(mantle_renderer PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Export library for linking with Rust
set_target_properties(mantle_renderer PROPERTIES
    PUBLIC_HEADER "vulkan_renderer.h;render_packet.h"
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
)

# Installation (optional, for development)
install(TARGETS mantle_renderer
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/mantle
)
