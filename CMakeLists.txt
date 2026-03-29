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

# Enable corrosion for Rust-C++ integration
include(FetchContent)
FetchContent_Declare(
    Corrosion
    GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
    GIT_TAG v0.4.5
)
FetchContent_MakeAvailable(Corrosion)

# C++ subproject (Vulkan Renderer) - Build first
add_subdirectory(cpp)

# Cargo project (MantleEngine Rust)
corrosion_import_crate(MANIFEST_PATH Cargo.toml CRATES mantle-engine)

# Link C++ library with Rust FFI if Rust target was created
# The mantle_renderer C++ library is available for linking with Rust extern "C" FFI calls
if(TARGET mantle-engine AND TARGET mantle_renderer)
    target_link_libraries(mantle-engine PRIVATE mantle_renderer)
endif()

# Optional: Build examples or tests
option(MANTLE_BUILD_EXAMPLES "Build example applications" OFF)
option(MANTLE_BUILD_TESTS "Build C++ unit tests" ON)

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
message(STATUS "Rust Integration:  Corrosion ${Corrosion_VERSION}")
message(STATUS "Vulkan Renderer:   Enabled")
message(STATUS "")
