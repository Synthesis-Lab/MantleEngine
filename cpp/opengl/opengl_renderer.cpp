#include "opengl_renderer.h"
#include <iostream>
#include <cstdarg>
#include <cmath>
#include <cmath>

// GLFW3 headers (conditional based on USE_GLFW3)
#ifdef USE_GLFW3
#include <GLFW/glfw3.h>
#else
// Stub for GLFW3 window when not available
struct GLFWwindow {
    int dummy;
};
#endif

// OpenGL headers (conditional based on USE_OPENGL)
#ifdef USE_OPENGL
#include <GL/gl.h>
#endif

// Always define stub GL functions for cases where GL is not available
// When USE_OPENGL is defined, the real functions from GL/gl.h are used
#ifndef glGenFramebuffers
#define glGenFramebuffers(n, fb) ((void)0)
#endif
#ifndef glBindFramebuffer
#define glBindFramebuffer(target, fb) ((void)0)
#endif
#ifndef glGenTextures
#define glGenTextures(n, t) ((void)0)
#endif
#ifndef glBindTexture
#define glBindTexture(target, t) ((void)0)
#endif
#ifndef glTexImage2D
#define glTexImage2D(...) ((void)0)
#endif
#ifndef glFramebufferTexture2D
#define glFramebufferTexture2D(...) ((void)0)
#endif
#ifndef glGenRenderbuffers
#define glGenRenderbuffers(n, rb) ((void)0)
#endif
#ifndef glBindRenderbuffer
#define glBindRenderbuffer(target, rb) ((void)0)
#endif
#ifndef glRenderbufferStorage
#define glRenderbufferStorage(...) ((void)0)
#endif
#ifndef glFramebufferRenderbuffer
#define glFramebufferRenderbuffer(...) ((void)0)
#endif
#ifndef glCheckFramebufferStatus
#define glCheckFramebufferStatus(target) 0x8CD5
#endif
#ifndef glGenSamplers
#define glGenSamplers(n, s) ((void)0)
#endif
#ifndef glSamplerParameteri
#define glSamplerParameteri(...) ((void)0)
#endif
#ifndef glGenVertexArrays
#define glGenVertexArrays(n, a) ((void)0)
#endif
#ifndef glGenBuffers
#define glGenBuffers(n, b) ((void)0)
#endif
#ifndef glDeleteBuffers
#define glDeleteBuffers(n, b) ((void)0)
#endif
#ifndef glDeleteVertexArrays
#define glDeleteVertexArrays(n, a) ((void)0)
#endif
#ifndef glDeleteFramebuffers
#define glDeleteFramebuffers(n, fb) ((void)0)
#endif
#ifndef glDeleteTextures
#define glDeleteTextures(n, t) ((void)0)
#endif
#ifndef glDeleteProgram
#define glDeleteProgram(p) ((void)0)
#endif
#ifndef glBindVertexArray
#define glBindVertexArray(a) ((void)0)
#endif
#ifndef glBindBuffer
#define glBindBuffer(target, b) ((void)0)
#endif
#ifndef glBufferData
#define glBufferData(target, size, data, usage) ((void)0)
#endif
#ifndef glVertexAttribPointer
#define glVertexAttribPointer(...) ((void)0)
#endif
#ifndef glEnableVertexAttribArray
#define glEnableVertexAttribArray(i) ((void)0)
#endif
#ifndef glEnable
#define glEnable(cap) ((void)0)
#endif
#ifndef glReadPixels
#define glReadPixels(...) ((void)0)
#endif
#ifndef glCreateProgram
#define glCreateProgram() 1
#endif
#ifndef glTexParameteri
#define glTexParameteri(...) ((void)0)
#endif
#ifndef glCreateShader
#define glCreateShader(type) 1
#endif
#ifndef glShaderSource
#define glShaderSource(...) ((void)0)
#endif
#ifndef glCompileShader
#define glCompileShader(shader) ((void)0)
#endif
#ifndef glGetShaderiv
inline void glGetShaderiv_stub(unsigned int shader, unsigned int pname, int *params) {
    if (params) {
        if (pname == GL_COMPILE_STATUS || pname == GL_INFO_LOG_LENGTH) {
            *params = (pname == GL_COMPILE_STATUS) ? 1 : 1;  // Success
        }
    }
}
#define glGetShaderiv(shader, pname, params) glGetShaderiv_stub(shader, pname, params)
#endif
#ifndef glGetShaderInfoLog
inline void glGetShaderInfoLog_stub(unsigned int shader, int maxLength, int *length, char *infoLog) {
    if (infoLog && maxLength > 0) infoLog[0] = '\0';
    if (length) *length = 0;
}
#define glGetShaderInfoLog(shader, maxLength, length, infoLog) glGetShaderInfoLog_stub(shader, maxLength, length, infoLog)
#endif
#ifndef glAttachShader
#define glAttachShader(...) ((void)0)
#endif
#ifndef glLinkProgram
#define glLinkProgram(program) ((void)0)
#endif
#ifndef glGetProgramiv
inline void glGetProgramiv_stub(unsigned int program, unsigned int pname, int *params) {
    if (params) {
        if (pname == GL_LINK_STATUS || pname == GL_INFO_LOG_LENGTH) {
            *params = (pname == GL_LINK_STATUS) ? 1 : 1;  // Success
        }
    }
}
#define glGetProgramiv(program, pname, params) glGetProgramiv_stub(program, pname, params)
#endif
#ifndef glGetProgramInfoLog
inline void glGetProgramInfoLog_stub(unsigned int program, int maxLength, int *length, char *infoLog) {
    if (infoLog && maxLength > 0) infoLog[0] = '\0';
    if (length) *length = 0;
}
#define glGetProgramInfoLog(program, maxLength, length, infoLog) glGetProgramInfoLog_stub(program, maxLength, length, infoLog)
#endif
#ifndef glDeleteShader
#define glDeleteShader(shader) ((void)0)
#endif
#ifndef glUseProgram
#define glUseProgram(program) ((void)0)
#endif
#ifndef glClearColor
#define glClearColor(...) ((void)0)
#endif
#ifndef glClear
#define glClear(mask) ((void)0)
#endif
#ifndef glDrawElements
#define glDrawElements(...) ((void)0)
#endif
#ifndef glfwInit
#define glfwInit() 1
#endif
#ifndef glfwWindowHint
#define glfwWindowHint(hint, value) ((void)0)
#endif
#ifndef glfwCreateWindow
// Return a non-null dummy window pointer when GLFW3 not available (cast int to pointer)
#define glfwCreateWindow(w, h, title, mon, share) ((GLFWwindow*)(0x1))
#endif
#ifndef glfwDestroyWindow
#define glfwDestroyWindow(w) ((void)0)
#endif
#ifndef glfwMakeContextCurrent
#define glfwMakeContextCurrent(w) ((void)0)
#endif
#ifndef glfwSwapInterval
#define glfwSwapInterval(i) ((void)0)
#endif
#ifndef glfwTerminate
#define glfwTerminate() ((void)0)
#endif

// Uniform function stubs for Phase 5b+5c (batch rendering with transforms)
#ifndef glGetUniformLocation
#define glGetUniformLocation(program, name) 0
#endif
#ifndef glUniformMatrix4fv
#define glUniformMatrix4fv(location, count, transpose, value) ((void)0)
#endif
#ifndef glUniform4f
#define glUniform4f(location, x, y, z, w) ((void)0)
#endif

// ============================================================================
// OpenGLRenderer Implementation
// ============================================================================

OpenGLRenderer::OpenGLRenderer()
    : window_(nullptr),
      window_width_(1024),
      window_height_(768),
      is_ready_(false),
      frame_number_(0),
      framebuffer_object_(0),
      color_texture_(0),
      depth_renderbuffer_(0),
      shader_program_(0),
      vertex_shader_(0),
      fragment_shader_(0),
      vertex_array_object_(0),
      quad_vertex_buffer_(0),
      quad_index_buffer_(0),
      instance_buffer_(0),
      texture_sampler_(0),
      placeholder_texture_(0),
      enable_frame_dump_(true),
      frame_dump_count_(1) {
    std::cout << "[MantleRenderer] OpenGL renderer constructed" << std::endl;
}

OpenGLRenderer::~OpenGLRenderer() {
    Shutdown();
    std::cout << "[MantleRenderer] OpenGL renderer destroyed" << std::endl;
}

bool OpenGLRenderer::Initialize() {
    std::cout << "[MantleRenderer] Initializing OpenGL renderer..." << std::endl;
    
    if (is_ready_) {
        SetError("Renderer already initialized");
        return false;
    }
    
    // Phase 1: Initialize GLFW
    if (!CreateGLFWWindow()) {
        SetError("Failed to create GLFW window");
        return false;
    }
    
    // Phase 2: Create OpenGL context
    if (!CreateGLContext()) {
        SetError("Failed to create OpenGL context");
        glfwDestroyWindow(window_);
        window_ = nullptr;
        return false;
    }
    
    // Phase 3: Create shader program
    if (!CreateShaderProgram()) {
        SetError("Failed to create shader program");
        return false;
    }
    
    // Phase 4: Create geometry (quad vertices + indices)
    if (!CreateQuadGeometry()) {
        SetError("Failed to create quad geometry");
        return false;
    }
    
    // Phase 5: Create framebuffer object for rendering
    if (!CreateFramebufferObject()) {
        SetError("Failed to create framebuffer object");
        return false;
    }
    
    // Phase 6: Create texture sampler
    if (!CreateTextureSampler()) {
        SetError("Failed to create texture sampler");
        return false;
    }
    
    // Phase 7: Create placeholder white texture
    if (!CreatePlaceholderTexture()) {
        SetError("Failed to create placeholder texture");
        return false;
    }
    
    is_ready_ = true;
    std::cout << "[MantleRenderer] OpenGL renderer initialized successfully" << std::endl;
    return true;
}

void OpenGLRenderer::Shutdown() {
    if (!is_ready_) {
        return;
    }
    
    std::cout << "[MantleRenderer] Shutting down OpenGL renderer..." << std::endl;
    
    // Phase 1: Delete GPU resources
    if (quad_vertex_buffer_) {
        glDeleteBuffers(1, &quad_vertex_buffer_);
        quad_vertex_buffer_ = 0;
    }
    
    if (quad_index_buffer_) {
        glDeleteBuffers(1, &quad_index_buffer_);
        quad_index_buffer_ = 0;
    }
    
    if (instance_buffer_) {
        glDeleteBuffers(1, &instance_buffer_);
        instance_buffer_ = 0;
    }
    
    if (vertex_array_object_) {
        glDeleteVertexArrays(1, &vertex_array_object_);
        vertex_array_object_ = 0;
    }
    
    if (shader_program_) {
        glDeleteProgram(shader_program_);
        shader_program_ = 0;
    }
    
    if (framebuffer_object_) {
        glDeleteFramebuffers(1, &framebuffer_object_);
        framebuffer_object_ = 0;
    }
    
    if (placeholder_texture_) {
        glDeleteTextures(1, &placeholder_texture_);
        placeholder_texture_ = 0;
    }
    
    // Phase 2: Destroy window
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    
    glfwTerminate();
    is_ready_ = false;
    std::cout << "[MantleRenderer] OpenGL shutdown complete" << std::endl;
}

bool OpenGLRenderer::IsReady() const {
    return is_ready_;
}

void OpenGLRenderer::SubmitRenderPacket(const RenderPacket* packet) {
    if (!is_ready_ || packet == nullptr) {
        SetError("Renderer not ready or packet is null");
        return;
    }
    
    // Phase 4: Queue packet for rendering
    // In a multi-threaded context, this would push to a queue
    // For now, render immediately
    RenderFrame(packet);
}

void OpenGLRenderer::WaitRender() {
    if (!is_ready_) {
        SetError("Renderer not ready");
        return;
    }
    
    // Phase 5: Render frame (will dump frame to PPM)
    // For testing: render empty frame or with pending packets
    RenderFrame(nullptr);
    
    frame_number_++;
}

uint32_t OpenGLRenderer::UploadBuffer(const void* data, uint32_t size) {
    if (!is_ready_ || data == nullptr || size == 0) {
        SetError("Invalid buffer upload parameters");
        return 0;
    }
    
    // Phase 4 (Skeleton) → Phase 5+ (Implementation)
    // Create a GPU buffer for the data
    // For now: stub that returns buffer ID 1
    std::cout << "[MantleRenderer] UploadBuffer called (size=" << size << " bytes)" << std::endl;
    return 1;
}

const char* OpenGLRenderer::GetLastError() const {
    return error_message_.c_str();
}

void OpenGLRenderer::RenderFrame(const RenderPacket* packet) {
    if (!is_ready_) {
        SetError("Renderer not ready");
        return;
    }
    
    // Phase 5a: Bind FBO for rendering
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_);
    glViewport(0, 0, window_width_, window_height_);
    
    // Clear screen with green background (Phase 5h signature color)
    glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use shader program
    glUseProgram(shader_program_);
    
    // Phase 5b: Batch render all sprites with their transforms
    if (packet && packet->sprite_count > 0) {
        // Get uniform locations
        int transform_loc = glGetUniformLocation(shader_program_, "transform_matrix");
        int color_loc = glGetUniformLocation(shader_program_, "sprite_color");
        
        // Render each sprite with its transform
        for (uint32_t i = 0; i < packet->sprite_count && i < packet->transform_count; ++i) {
            const TransformPacket& transform = packet->transforms[i];
            const SpritePacket& sprite = packet->sprites[i];
            
            // Phase 5c: Calculate 2D TRS matrix
            // Create transform matrix: T * R * S
            float angle_rad = transform.rotation * 3.14159265f / 180.0f;
            float cos_rot = ::cos(angle_rad);
            float sin_rot = ::sin(angle_rad);
            
            // 2D TRS matrix (4x4 for compatibility)
            float matrix[16] = {
                // Scale and Rotation combined
                transform.scale_x * cos_rot, -sin_rot, 0.0f, 0.0f,
                sin_rot, transform.scale_y * cos_rot, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                // Translation
                transform.position_x, transform.position_y, 0.0f, 1.0f
            };
            
            // Set uniforms for this sprite
            glUniformMatrix4fv(transform_loc, 1, GL_FALSE, matrix);
            
            // Decode color from RGBA packed format (0xRRGGBBAA)
            float r = ((sprite.color >> 24) & 0xFF) / 255.0f;
            float g = ((sprite.color >> 16) & 0xFF) / 255.0f;
            float b = ((sprite.color >> 8) & 0xFF) / 255.0f;
            float a = (sprite.color & 0xFF) / 255.0f;
            glUniform4f(color_loc, r, g, b, a);
            
            // Bind VAO and render quad
            glBindVertexArray(vertex_array_object_);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
            
            std::cout << "[MantleRenderer] Sprite " << i << ": pos=(" << transform.position_x 
                      << "," << transform.position_y << ") rot=" << transform.rotation 
                      << "° color=#" << std::hex << sprite.color << std::dec << std::endl;
        }
        glBindVertexArray(0);
    } else {
        // Fallback: render single green quad if no sprites provided (Phase 5a compatibility)
        float default_matrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        int transform_loc = glGetUniformLocation(shader_program_, "transform_matrix");
        int color_loc = glGetUniformLocation(shader_program_, "sprite_color");
        
        glUniformMatrix4fv(transform_loc, 1, GL_FALSE, default_matrix);
        glUniform4f(color_loc, 0.0f, 0.5f, 0.0f, 1.0f);  // Phase 5a green
        
        glBindVertexArray(vertex_array_object_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
        glBindVertexArray(0);
    }
    
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Log frame info
    if (packet) {
        std::cout << "[MantleRenderer] Frame " << packet->frame_number 
                  << ": " << packet->transform_count << " transforms, "
                  << packet->sprite_count << " sprites" << std::endl;
    }
    
    // Always dump the frame (for testing/visualization)
    DumpCurrentFrame();
}

// ============================================================================
// Helper Methods
// ============================================================================

bool OpenGLRenderer::CreateGLFWWindow() {
    std::cout << "[MantleRenderer] Creating GLFW window..." << std::endl;
    
    if (!glfwInit()) {
        SetError("Failed to initialize GLFW");
        return false;
    }
    
    // Require OpenGL 4.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // Headless mode for testing
    
    window_ = glfwCreateWindow(window_width_, window_height_, 
                               "MantleEngine - OpenGL", nullptr, nullptr);
    if (!window_) {
        SetError("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }
    
    std::cout << "[MantleRenderer] GLFW window created (" << window_width_ 
              << "x" << window_height_ << ")" << std::endl;
    return true;
}

bool OpenGLRenderer::CreateGLContext() {
    std::cout << "[MantleRenderer] Creating OpenGL context..." << std::endl;
    
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(0);  // No vsync for testing
    
    // Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    std::cout << "[MantleRenderer] OpenGL context created" << std::endl;
    return true;
}

bool OpenGLRenderer::CreateShaderProgram() {
    std::cout << "[MantleRenderer] Creating shader program..." << std::endl;
    
    // Phase 5b/5c: Compile shaders with transform support
    // Vertex shader - apply 2D TRS transformation to quad vertices
    const char* vertex_source = R"glsl(
        #version 430 core
        layout(location = 0) in vec2 position;
        
        // Per-instance transform (Phase 5c)
        uniform mat4 transform_matrix;
        uniform vec4 sprite_color;  // RGBA color for this sprite
        
        out VS_OUT {
            vec4 vertex_color;
        } vs_out;
        
        void main() {
            // Apply transform matrix to quad position
            vec4 world_pos = transform_matrix * vec4(position, 0.0, 1.0);
            gl_Position = world_pos;
            
            // Pass color to fragment shader
            vs_out.vertex_color = sprite_color;
        }
    )glsl";
    
    // Fragment shader - output sprite color (Phase 5b signature)
    const char* fragment_source = R"glsl(
        #version 430 core
        
        in VS_OUT {
            vec4 vertex_color;
        } fs_in;
        
        out vec4 FragColor;
        
        void main() {
            // Use per-sprite color (from SpritePacket)
            FragColor = fs_in.vertex_color;
        }
    )glsl";
    
    // Compile vertex shader
    vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_, 1, &vertex_source, nullptr);
    glCompileShader(vertex_shader_);
    
    // Check vertex shader compilation
    int vertex_success;
    char vertex_log[512];
    glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &vertex_success);
    if (!vertex_success) {
        glGetShaderInfoLog(vertex_shader_, 512, nullptr, vertex_log);
        SetError("Vertex shader compilation failed: %s", vertex_log);
        return false;
    }
    
    // Compile fragment shader
    fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_, 1, &fragment_source, nullptr);
    glCompileShader(fragment_shader_);
    
    // Check fragment shader compilation
    int fragment_success;
    char fragment_log[512];
    glGetShaderiv(fragment_shader_, GL_COMPILE_STATUS, &fragment_success);
    if (!fragment_success) {
        glGetShaderInfoLog(fragment_shader_, 512, nullptr, fragment_log);
        SetError("Fragment shader compilation failed: %s", fragment_log);
        return false;
    }
    
    // Link program
    shader_program_ = glCreateProgram();
    glAttachShader(shader_program_, vertex_shader_);
    glAttachShader(shader_program_, fragment_shader_);
    glLinkProgram(shader_program_);
    
    // Check program linking
    int link_success;
    char link_log[512];
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &link_success);
    if (!link_success) {
        glGetProgramInfoLog(shader_program_, 512, nullptr, link_log);
        SetError("Shader program linking failed: %s", link_log);
        return false;
    }
    
    // Clean up individual shaders (no longer needed after linking)
    glDeleteShader(vertex_shader_);
    glDeleteShader(fragment_shader_);
    
    std::cout << "[MantleRenderer] Shader program created successfully" << std::endl;
    return true;
}

bool OpenGLRenderer::CreateQuadGeometry() {
    std::cout << "[MantleRenderer] Creating quad geometry..." << std::endl;
    
    // Phase 5g: Create VAO + VBO for quad vertices
    glGenVertexArrays(1, &vertex_array_object_);
    glGenBuffers(1, &quad_vertex_buffer_);
    glGenBuffers(1, &quad_index_buffer_);
    
    // Bind VAO
    glBindVertexArray(vertex_array_object_);
    
    // Define quad vertices (32-bit floats, 2D position)
    float quad_vertices[] = {
        -0.5f, -0.5f,  // Bottom-left
         0.5f, -0.5f,  // Bottom-right
         0.5f,  0.5f,  // Top-right
        -0.5f,  0.5f   // Top-left
    };
    
    // Define quad indices (2 triangles, 6 indices total)
    uint16_t quad_indices[] = {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };
    
    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    
    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_index_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
    
    // Define vertex attributes (position only for now)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    std::cout << "[MantleRenderer] Quad geometry created" << std::endl;
    return true;
}

bool OpenGLRenderer::CreateFramebufferObject() {
    std::cout << "[MantleRenderer] Creating framebuffer object..." << std::endl;
    
    // Phase 5a: Create FBO + attachments
    glGenFramebuffers(1, &framebuffer_object_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_);
    
    // Create color texture attachment
    glGenTextures(1, &color_texture_);
    glBindTexture(GL_TEXTURE_2D, color_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width_, window_height_, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                          GL_TEXTURE_2D, color_texture_, 0);
    
    // Create depth renderbuffer
    glGenRenderbuffers(1, &depth_renderbuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_renderbuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width_, window_height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
                             GL_RENDERBUFFER, depth_renderbuffer_);
    
    // Check framebuffer completeness
    uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    const uint32_t GL_FRAMEBUFFER_COMPLETE_VAL = 0x8CD5;  // GL_FRAMEBUFFER_COMPLETE value
    if (status != GL_FRAMEBUFFER_COMPLETE_VAL) {
        char error_buf[256];
        snprintf(error_buf, sizeof(error_buf), "Framebuffer incomplete: 0x%x", status);
        SetError("%s", error_buf);
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    std::cout << "[MantleRenderer] Framebuffer object created" << std::endl;
    return true;
}

bool OpenGLRenderer::CreateTextureSampler() {
    std::cout << "[MantleRenderer] Creating texture sampler..." << std::endl;
    
    // Phase 5f: Create sampler object
    glGenSamplers(1, &texture_sampler_);
    glSamplerParameteri(texture_sampler_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(texture_sampler_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(texture_sampler_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(texture_sampler_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    std::cout << "[MantleRenderer] Texture sampler created" << std::endl;
    return true;
}

bool OpenGLRenderer::CreatePlaceholderTexture() {
    std::cout << "[MantleRenderer] Creating placeholder texture..." << std::endl;
    
    // Phase 5f: Create 1x1 white texture
    glGenTextures(1, &placeholder_texture_);
    glBindTexture(GL_TEXTURE_2D, placeholder_texture_);
    
    uint8_t white_pixel[] = { 255, 255, 255, 255 };  // RGBA white
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white_pixel);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    std::cout << "[MantleRenderer] Placeholder texture created" << std::endl;
    return true;
}

void OpenGLRenderer::DumpCurrentFrame() {
    if (!enable_frame_dump_) {
        return;
    }
    
    // Phase 5d: Read framebuffer and dump to PPM
    if (pixel_buffer_.empty()) {
        pixel_buffer_.resize(window_width_ * window_height_ * 4, 0);  // RGBA
    }
    
    // Bind framebuffer and read pixels
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_object_);
    glReadPixels(0, 0, window_width_, window_height_, GL_RGBA, GL_UNSIGNED_BYTE, 
                 pixel_buffer_.data());
    
    // Generate filename and save
    char filename[256];
    snprintf(filename, sizeof(filename), "output/frame_%04u.ppm", frame_dump_count_);
    
    if (SaveFrameToPPM(filename, window_width_, window_height_)) {
        std::cout << "[MantleRenderer] Frame dumped: " << filename << std::endl;
        frame_dump_count_++;
    } else {
        std::cout << "[MantleRenderer] Failed to dump frame: " << filename << std::endl;
    }
}

bool OpenGLRenderer::SaveFrameToPPM(const char* filename, uint32_t width, uint32_t height) {
    // Ensure output directory exists
    system("mkdir -p output");
    
    // Open file for binary writing
    FILE* file = fopen(filename, "wb");
    if (!file) {
        SetError("Failed to open file for writing: %s", filename);
        return false;
    }
    
    // Write PPM header (P6 = binary RGB format)
    fprintf(file, "P6\n%u %u\n255\n", width, height);
    
    // Write pixel data (RGB triplets, converting from RGBA)
    // Note: PPM expects row-major order (top-to-bottom)
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            uint32_t pixel_idx = y * width + x;
            if (pixel_idx * 4 + 3 < pixel_buffer_.size()) {
                uint8_t r = pixel_buffer_[pixel_idx * 4 + 0];
                uint8_t g = pixel_buffer_[pixel_idx * 4 + 1];
                uint8_t b = pixel_buffer_[pixel_idx * 4 + 2];
                
                fputc(r, file);
                fputc(g, file);
                fputc(b, file);
            }
        }
    }
    
    fclose(file);
    return true;
}

void OpenGLRenderer::SetError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    error_message_ = buffer;
    
    va_end(args);
    
    // Also print to stderr for debugging
    std::cerr << "[MantleRenderer ERROR] " << error_message_ << std::endl;
}
