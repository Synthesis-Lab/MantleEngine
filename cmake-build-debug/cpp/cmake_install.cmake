# Install script for directory: /home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cmake-build-debug/cpp/libmantle_renderer.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mantle" TYPE FILE FILES
    "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/render_packet.h"
    "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/vulkan_renderer.h"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mantle/core" TYPE FILE FILES
    "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/core/engine.h"
    "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/core/module.h"
    "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/core/time_step.h"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mantle/include" TYPE FILE FILES
    "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/include/renderer.h"
    "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/include/opengl_renderer.h"
    "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/include/runtime.h"
    "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/include/mantle_engine.h"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mantle/modules/input" TYPE FILE FILES "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/modules/input/input_system.h")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mantle/modules/physics" TYPE FILE FILES "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/modules/physics/physics_world.h")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mantle/modules/sound" TYPE FILE FILES "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/modules/sound/sound_system.h")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mantle/runtime" TYPE FILE FILES "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/runtime/runtime.h")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cmake-build-debug/cpp/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
