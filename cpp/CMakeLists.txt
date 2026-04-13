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
add_library(mantle_renderer)

target_sources(mantle_renderer
    PRIVATE
        core/engine.cpp
        runtime/runtime.cpp
        modules/input/input_system.cpp
        modules/sound/sound_system.cpp
        modules/physics/physics_world.cpp
        vulkan_renderer.cpp
        opengl/opengl_renderer.cpp
        renderer.cpp
        vulkan/vulkan_stub.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
            include/renderer.h
            include/opengl_renderer.h
            include/runtime.h
            include/mantle_engine.h
            render_packet.h
            vulkan_renderer.h
            core/engine.h
            core/module.h
            core/time_step.h
            runtime/runtime.h
            modules/input/input_system.h
            modules/sound/sound_system.h
            modules/physics/physics_world.h
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
    target_compile_options(mantle_renderer PRIVATE /W4 /permissive-)
else()
    target_compile_options(mantle_renderer PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
    )
endif()

if(NOT MSVC)
    # Third-party-like stub and legacy backend files are intentionally permissive.
    set_source_files_properties(vulkan/vulkan_stub.cpp PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter")
    set_source_files_properties(vulkan_renderer.cpp PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-conversion;-Wno-sign-conversion")
    set_source_files_properties(opengl/opengl_renderer.cpp PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-unused-variable;-Wno-conversion;-Wno-sign-conversion;-Wno-float-conversion")
endif()

set_target_properties(mantle_renderer PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
)

if(MANTLE_BUILD_EXAMPLES)
    add_executable(mantle_demo
        examples/main.cpp
    )

    target_link_libraries(mantle_demo PRIVATE mantle_renderer)
endif()

# Installation (optional, for development)
install(TARGETS mantle_renderer
    LIBRARY DESTINATION lib
    FILE_SET HEADERS DESTINATION include/mantle
)
