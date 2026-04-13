# MantleEngine

MantleEngine is a C++23 game engine built with CMake.

## Current Direction

- Core language/runtime: C++23
- Build system: CMake
- Rendering backends: Vulkan and OpenGL Core
- Next milestones: Input, Sound, Physics modules
- Scripting plan: Lua via sol2 (after core modules are finalized)

## Build (Linux)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DMANTLE_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

If `cmake` is not installed on your machine, install it first and rerun the commands.
