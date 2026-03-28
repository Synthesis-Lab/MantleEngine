#version 450

// Vertex attributes
layout(location = 0) in vec2 in_position;      // Quad vertex position (-0.5 to 0.5)
layout(location = 1) in vec2 in_texcoord;      // Quad texture coordinate (0 to 1)

// Instance attributes (per-sprite)
layout(location = 2) in vec2 in_world_pos;     // Transform world position
layout(location = 3) in float in_rotation;     // Transform rotation in degrees
layout(location = 4) in vec2 in_scale;         // Transform scale
layout(location = 5) in vec4 in_uv_bounds;     // UV min.xy and max.zw for animation
layout(location = 6) in vec4 in_color;         // Color modulation RGBA

// Output to fragment shader
layout(location = 0) out vec2 frag_texcoord;
layout(location = 1) out vec4 frag_color;

void main() {
    // Apply 2D transformation: rotation + scale + translation
    
    // Convert rotation from degrees to radians
    float angle = radians(in_rotation);
    float cos_angle = cos(angle);
    float sin_angle = sin(angle);
    
    // Apply rotation
    float rotated_x = in_position.x * cos_angle - in_position.y * sin_angle;
    float rotated_y = in_position.x * sin_angle + in_position.y * cos_angle;
    
    // Apply scale
    float scaled_x = rotated_x * in_scale.x;
    float scaled_y = rotated_y * in_scale.y;
    
    // Apply translation from world position
    float world_x = scaled_x + in_world_pos.x;
    float world_y = scaled_y + in_world_pos.y;
    
    // Convert world coordinates to NDC (-1 to 1)
    // Assuming 1024x768 screen resolution
    // You may want to make this a push constant for flexibility
    float screen_width = 1024.0;
    float screen_height = 768.0;
    
    float ndc_x = (world_x / (screen_width * 0.5)) - 1.0;
    float ndc_y = 1.0 - (world_y / (screen_height * 0.5));
    
    gl_Position = vec4(ndc_x, ndc_y, 0.0, 1.0);
    
    // Map texture coordinates using UV bounds
    // in_texcoord is 0-1 for quad vertices
    // in_uv_bounds.xy is min UV, in_uv_bounds.zw is max UV
    frag_texcoord = mix(in_uv_bounds.xy, in_uv_bounds.zw, in_texcoord);
    
    // Pass color to fragment shader
    frag_color = in_color;
}
