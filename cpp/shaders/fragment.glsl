#version 450

// Input from vertex shader
layout(location = 0) in vec2 frag_texcoord;
layout(location = 1) in vec4 frag_color;

// Texture descriptor (sampler)
layout(binding = 0) uniform sampler2D tex_sampler;

// Output color
layout(location = 0) out vec4 outColor;

void main() {
    // Sample texture at interpolated UV coordinates
    vec4 tex_color = texture(tex_sampler, frag_texcoord);
    
    // Apply color modulation from sprite packet
    // Multiply texture color by the per-sprite color
    outColor = tex_color * frag_color;
    
    // Handle transparency/alpha
    // If alpha is 0, discard the fragment for proper transparency
    if (outColor.a < 0.01) {
        discard;
    }
}
