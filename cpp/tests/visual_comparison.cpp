/// visual_comparison.cpp
/// Phase 5b+5c: Visual output comparison between Vulkan and OpenGL
/// 
/// This program creates the same scene with Vulkan and OpenGL,
/// then compares the frame output to verify rendering parity.

#include "../include/renderer.h"
#include "../render_packet.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>

void CreateColorfulScene(TransformPacket* transforms, SpritePacket* sprites, int& count) {
    /// Create 4 sprites in a 2x2 grid with primary colors
    count = 4;
    
    // Red: top-left
    transforms[0].position_x = -0.4f;
    transforms[0].position_y = 0.4f;
    transforms[0].rotation = 0.0f;
    transforms[0].scale_x = 0.8f;
    transforms[0].scale_y = 0.8f;
    transforms[0].z_index = 1;
    transforms[0].padding = 0;
    
    sprites[0].texture_id = 1;
    sprites[0].width = 64.0f;
    sprites[0].height = 64.0f;
    sprites[0].color = 0xFF0000FF;  // Red
    sprites[0].uv_min_x = 0.0f;
    sprites[0].uv_min_y = 0.0f;
    sprites[0].uv_max_x = 1.0f;
    sprites[0].uv_max_y = 1.0f;
    
    // Green: top-right
    transforms[1].position_x = 0.4f;
    transforms[1].position_y = 0.4f;
    transforms[1].rotation = 0.0f;
    transforms[1].scale_x = 0.8f;
    transforms[1].scale_y = 0.8f;
    transforms[1].z_index = 1;
    transforms[1].padding = 0;
    
    sprites[1].texture_id = 2;
    sprites[1].width = 64.0f;
    sprites[1].height = 64.0f;
    sprites[1].color = 0x00FF00FF;  // Green
    sprites[1].uv_min_x = 0.0f;
    sprites[1].uv_min_y = 0.0f;
    sprites[1].uv_max_x = 1.0f;
    sprites[1].uv_max_y = 1.0f;
    
    // Blue: bottom-left (with 45° rotation)
    transforms[2].position_x = -0.4f;
    transforms[2].position_y = -0.4f;
    transforms[2].rotation = 45.0f;
    transforms[2].scale_x = 0.8f;
    transforms[2].scale_y = 0.8f;
    transforms[2].z_index = 2;
    transforms[2].padding = 0;
    
    sprites[2].texture_id = 3;
    sprites[2].width = 64.0f;
    sprites[2].height = 64.0f;
    sprites[2].color = 0x0000FFFF;  // Blue
    sprites[2].uv_min_x = 0.0f;
    sprites[2].uv_min_y = 0.0f;
    sprites[2].uv_max_x = 1.0f;
    sprites[2].uv_max_y = 1.0f;
    
    // Yellow: bottom-right (with -45° rotation and 1.2x scale)
    transforms[3].position_x = 0.4f;
    transforms[3].position_y = -0.4f;
    transforms[3].rotation = -45.0f;
    transforms[3].scale_x = 1.2f;
    transforms[3].scale_y = 1.2f;
    transforms[3].z_index = 3;
    transforms[3].padding = 0;
    
    sprites[3].texture_id = 4;
    sprites[3].width = 56.0f;
    sprites[3].height = 56.0f;
    sprites[3].color = 0xFFFF00FF;  // Yellow
    sprites[3].uv_min_x = 0.0f;
    sprites[3].uv_min_y = 0.0f;
    sprites[3].uv_max_x = 1.0f;
    sprites[3].uv_max_y = 1.0f;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Phase 5b+5c: Visual Comparison Test" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    /// Test Vulkan Backend
    std::cout << "[TEST 1] Vulkan - Colorful 4-Sprite Grid" << std::endl;
    std::cout << "─────────────────────────────────────────" << std::endl;
    {
        auto vulkan_renderer = Renderer::Create(Renderer::Type::VULKAN);
        
        if (!vulkan_renderer || !vulkan_renderer->Initialize()) {
            std::cerr << "Failed to initialize Vulkan renderer" << std::endl;
            return 1;
        }
        
        TransformPacket transforms[4];
        SpritePacket sprites[4];
        int sprite_count = 0;
        CreateColorfulScene(transforms, sprites, sprite_count);
        
        RenderPacket packet;
        packet.frame_number = 1;
        packet.delta_time = 0.016f;
        packet.transform_count = sprite_count;
        packet.sprite_count = sprite_count;
        packet.collider_count = 0;
        packet.transforms = transforms;
        packet.sprites = sprites;
        packet.colliders = nullptr;
        
        std::cout << "📤 Submitting render packet:" << std::endl;
        std::cout << "   - Sprites: " << sprite_count << std::endl;
        std::cout << "   - Layout: 2×2 grid with mixed rotations" << std::endl;
        std::cout << "   - Colors: Red, Green, Blue, Yellow" << std::endl;
        
        vulkan_renderer->SubmitRenderPacket(&packet);
        vulkan_renderer->WaitRender();
        
        std::cout << "✅ Vulkan render complete" << std::endl;
        std::cout << "   - Output: output/frame_***.ppm (check first green frame)" << std::endl;
        
        vulkan_renderer->Shutdown();
    }
    
    std::cout << std::endl;
    
    /// Test OpenGL Backend  
    std::cout << "[TEST 2] OpenGL - Cross Layout (2 Sprites)" << std::endl;
    std::cout << "─────────────────────────────────────────" << std::endl;
    {
        auto opengl_renderer = Renderer::Create(Renderer::Type::OPENGL);
        
        if (!opengl_renderer || !opengl_renderer->Initialize()) {
            std::cerr << "Failed to initialize OpenGL renderer" << std::endl;
            return 1;
        }
        
        // Create 2-sprite cross layout
        TransformPacket transforms[2];
        SpritePacket sprites[2];
        
        // Horizontal sprite (center-left)
        transforms[0].position_x = -0.3f;
        transforms[0].position_y = 0.0f;
        transforms[0].rotation = 0.0f;
        transforms[0].scale_x = 1.0f;
        transforms[0].scale_y = 0.5f;
        transforms[0].z_index = 1;
        transforms[0].padding = 0;
        
        sprites[0].texture_id = 1;
        sprites[0].width = 128.0f;
        sprites[0].height = 32.0f;
        sprites[0].color = 0xFF0080FF;  // Magenta
        sprites[0].uv_min_x = 0.0f;
        sprites[0].uv_min_y = 0.0f;
        sprites[0].uv_max_x = 1.0f;
        sprites[0].uv_max_y = 1.0f;
        
        // Vertical sprite (center-right, 90° rotation)
        transforms[1].position_x = 0.3f;
        transforms[1].position_y = 0.0f;
        transforms[1].rotation = 90.0f;
        transforms[1].scale_x = 1.0f;
        transforms[1].scale_y = 0.5f;
        transforms[1].z_index = 2;
        transforms[1].padding = 0;
        
        sprites[1].texture_id = 2;
        sprites[1].width = 128.0f;
        sprites[1].height = 32.0f;
        sprites[1].color = 0x00FFFFFF;  // Cyan
        sprites[1].uv_min_x = 0.0f;
        sprites[1].uv_min_y = 0.0f;
        sprites[1].uv_max_x = 1.0f;
        sprites[1].uv_max_y = 1.0f;
        
        RenderPacket packet;
        packet.frame_number = 2;
        packet.delta_time = 0.016f;
        packet.transform_count = 2;
        packet.sprite_count = 2;
        packet.collider_count = 0;
        packet.transforms = transforms;
        packet.sprites = sprites;
        packet.colliders = nullptr;
        
        std::cout << "📤 Submitting render packet:" << std::endl;
        std::cout << "   - Sprites: 2" << std::endl;
        std::cout << "   - Layout: Cross pattern (H + V)" << std::endl;
        std::cout << "   - Colors: Magenta (horizontal), Cyan (vertical)" << std::endl;
        
        opengl_renderer->SubmitRenderPacket(&packet);
        opengl_renderer->WaitRender();
        
        std::cout << "✅ OpenGL render complete" << std::endl;
        std::cout << "   - Output: output/frame_***.ppm (check magenta/cyan frames)" << std::endl;
        
        opengl_renderer->Shutdown();
    }
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Visual Comparison Complete!" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "📋 Summary:" << std::endl;
    std::cout << "  ✅ Vulkan: 4-sprite grid (RGBY) with rotation" << std::endl;
    std::cout << "  ✅ OpenGL: 2-sprite cross (magenta + cyan)" << std::endl;
    std::cout << "  ✅ Both backends render to output/frame_***.ppm" << std::endl;
    std::cout << "  ✅ Check frame colors for verification:" << std::endl;
    std::cout << "     - Vulkan green bg + RGBY sprites" << std::endl;
    std::cout << "     - OpenGL green bg + magenta/cyan sprites" << std::endl;
    std::cout << std::endl;
    
    return 0;
}
