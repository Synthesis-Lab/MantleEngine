#include "renderer.h"
#include "vulkan_renderer.h"
#include "opengl_renderer.h"
#include <iostream>

/// Factory function - creates appropriate renderer instance
std::unique_ptr<Renderer> Renderer::Create(Type type) {
    // Determine which backend to use
    Type target_type = type;
    
    if (type == Type::AUTO) {
        // Auto-detect: Try Vulkan first, fallback to OpenGL
        std::cout << "[MantleRenderer] Auto-detecting best graphics backend...\n";
        target_type = Type::VULKAN;  // Default to Vulkan for now
    }
    
    // Create renderer instance based on type
    switch (target_type) {
        case Type::VULKAN: {
            std::cout << "[MantleRenderer] Creating Vulkan renderer\n";
            return std::make_unique<VulkanRenderer>();
        }
        case Type::OPENGL: {
            std::cout << "[MantleRenderer] Creating OpenGL renderer\n";
            return std::make_unique<OpenGLRenderer>();
        }
        case Type::DIRECTX12: {
            std::cerr << "[MantleRenderer] DirectX12 not yet supported\n";
            return nullptr;
        }
        case Type::METAL: {
            std::cerr << "[MantleRenderer] Metal not yet supported\n";
            return nullptr;
        }
        default:
            std::cerr << "[MantleRenderer] Unknown renderer type\n";
            return nullptr;
    }
}
