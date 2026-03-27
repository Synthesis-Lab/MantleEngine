cmake_minimum_required(VERSION 3.16)
project(MantleEngine VERSION 0.4.0 LANGUAGES C CXX)

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

# Cargo project (MantleEngine Rust)
corrosion_import_crate(MANIFEST_PATH Cargo.toml CRATES mantle_engine)

# C++ subproject (Vulkan Renderer)
add_subdirectory(cpp)

# Link C++ library with Rust FFI
# The mantle_renderer C++ library is available for linking with Rust extern "C" FFI calls
target_link_libraries(mantle_engine PRIVATE mantle_renderer)

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
message(STATUS "C++ Standard:      17")
message(STATUS "Rust Integration:  Corrosion ${Corrosion_VERSION}")
message(STATUS "Vulkan Renderer:   Enabled")
message(STATUS "")
