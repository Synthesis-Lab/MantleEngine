cmake_minimum_required(VERSION 3.25)
project(MantleEngine VERSION 0.4.0 LANGUAGES C CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Optional: Build examples or tests
option(MANTLE_BUILD_EXAMPLES "Build example applications" OFF)
option(MANTLE_BUILD_TESTS "Build C++ unit tests" ON)

# C++ subproject
add_subdirectory(cpp)

if(MANTLE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(cpp/tests)
endif()

# Summary
message(STATUS "")
message(STATUS "MantleEngine v0.4.0 Configuration")
message(STATUS "==================================")
message(STATUS "Build Type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:      ${CMAKE_CXX_STANDARD}")
message(STATUS "Engine Runtime:    C++23")
message(STATUS "Renderer:          Vulkan/OpenGL")
message(STATUS "")
