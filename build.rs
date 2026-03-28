// build.rs - Build script for MantleEngine
// Handles C++ Vulkan renderer linking and FFI setup

use std::env;
use std::path::Path;

fn main() {
    // Phase 4: C++ FFI Integration
    // When building with CMake (recommended), set MANTLE_CPP_BUILD_DIR environment variable
    // For standalone builds, C++ linking is optional
    
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let cpp_lib_path = Path::new(&manifest_dir).join("cpp").join("lib");
    
    // Check if we're being built with CMake
    // CMake sets MANTLE_CPP_BUILD_DIR when integrated
    if let Ok(cpp_build_dir) = env::var("MANTLE_CPP_BUILD_DIR") {
        // Link against CMake-built C++ library
        println!("cargo:rustc-link-search=native={}/cpp", cpp_build_dir);
        println!("cargo:rustc-link-lib=static=mantle_renderer");
        println!("cargo:rustc-link-search=native={}/cpp/build", cpp_build_dir);
        
        // Vulkan linking
        #[cfg(target_os = "windows")]
        {
            if let Ok(vulkan_sdk) = env::var("VULKAN_SDK") {
                println!("cargo:rustc-link-search=native={}\\Lib", vulkan_sdk);
                println!("cargo:rustc-link-lib=vulkan-1");
            }
        }
        
        #[cfg(target_os = "linux")]
        {
            println!("cargo:rustc-link-search=native=/usr/lib");
            println!("cargo:rustc-link-search=native=/usr/local/lib");
            println!("cargo:rustc-link-lib=vulkan");
        }
        
        #[cfg(target_os = "macos")]
        {
            println!("cargo:rustc-link-search=native=/usr/local/lib");
            println!("cargo:rustc-link-lib=vulkan");
        }
        
        // C++ standard library
        #[cfg(target_os = "macos")]
        {
            println!("cargo:rustc-link-lib=c++");
        }
        
        #[cfg(target_env = "msvc")]
        {
            // MSVC already links C++ runtime
        }
        
        #[cfg(all(not(target_os = "macos"), not(target_env = "msvc")))]
        {
            println!("cargo:rustc-link-lib=stdc++");
        }
    } else {
        // Standalone build mode (for testing)
        // Only link if library has been pre-built
        if cpp_lib_path.exists() {
            println!("cargo:rustc-link-search=native={}", cpp_lib_path.display());
            println!("cargo:rustc-link-lib=static=mantle_renderer");
        } else {
            // Library not yet built; tests will run without C++ FFI
            // Phase 4 requires CMake integration for full C++ linking
            println!("cargo:warning=C++ VulkanRenderer not linked (Phase 4 requires CMake build)");
            println!("cargo:warning=For full Phase 4 integration: cmake -B build . && cmake --build build");
        }
    }
    
    // Rerun build script if CMakeLists.txt changes
    println!("cargo:rerun-if-changed=CMakeLists.txt");
    println!("cargo:rerun-if-changed=cpp/CMakeLists.txt");
    println!("cargo:rerun-if-changed=cpp/vulkan_renderer.h");
    println!("cargo:rerun-if-changed=cpp/vulkan_renderer.cpp");
    println!("cargo:rerun-if-changed=cpp/render_packet.h");
    
    println!("cargo:rustc-cfg=phase_4");
}
