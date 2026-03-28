#ifndef MANTLE_RENDER_PACKET_H
#define MANTLE_RENDER_PACKET_H

#include <cstdint>
#include <cstring>

/// TransformPacket - Byte-perfect mirror of Rust repr(C) struct
/// Size: 28 bytes (field layout must match Rust exactly)
struct TransformPacket {
    float position_x;      // +0  bytes
    float position_y;      // +4  bytes
    float rotation;        // +8  bytes (degrees)
    float scale_x;         // +12 bytes
    float scale_y;         // +16 bytes
    int32_t z_index;       // +20 bytes
    uint32_t padding;      // +24 bytes (for alignment)
    
    TransformPacket() = default;
    
    TransformPacket(float px, float py, float rot, float sx, float sy, int32_t z)
        : position_x(px), position_y(py), rotation(rot), 
          scale_x(sx), scale_y(sy), z_index(z), padding(0) {}
};

static_assert(sizeof(TransformPacket) == 28, "TransformPacket must be 28 bytes");

/// SpritePacket - Byte-perfect mirror of Rust repr(C) struct
/// Size: 36 bytes
struct SpritePacket {
    uint32_t texture_id;       // +0  bytes
    float width;               // +4  bytes
    float height;              // +8  bytes
    float color_r;             // +12 bytes
    float color_g;             // +16 bytes
    float color_b;             // +20 bytes
    float color_a;             // +24 bytes
    float uv_min_x;            // +28 bytes
    float uv_min_y;            // +32 bytes
    
    SpritePacket() = default;
    
    SpritePacket(uint32_t tex_id, float w, float h, 
                 float r, float g, float b, float a,
                 float uv_x, float uv_y)
        : texture_id(tex_id), width(w), height(h),
          color_r(r), color_g(g), color_b(b), color_a(a),
          uv_min_x(uv_x), uv_min_y(uv_y) {}
};

static_assert(sizeof(SpritePacket) == 36, "SpritePacket must be 36 bytes");

/// ColliderPacket - Byte-perfect mirror of Rust repr(C) struct
/// Size: 28 bytes
struct ColliderPacket {
    uint32_t shape_type;       // +0  bytes (0 = AABB, 1 = Circle)
    float width;               // +4  bytes (AABB: width, Circle: unused)
    float height;              // +8  bytes (AABB: height, Circle: unused)
    float radius;              // +12 bytes (Circle: radius, AABB: unused)
    float offset_x;            // +16 bytes
    float offset_y;            // +20 bytes
    uint32_t padding;          // +24 bytes (alignment)
    
    ColliderPacket() = default;
    
    ColliderPacket(uint32_t shape, float w, float h, float r, float ox, float oy)
        : shape_type(shape), width(w), height(h), radius(r), 
          offset_x(ox), offset_y(oy), padding(0) {}
};

static_assert(sizeof(ColliderPacket) == 28, "ColliderPacket must be 28 bytes");

/// RenderPacket - Frame data and array pointers for batch rendering
/// Contains frame metadata and pointers to transform/sprite/collider arrays
struct RenderPacket {
    uint32_t frame_number;              // +0
    float delta_time;                   // +4
    uint32_t transform_count;           // +8
    uint32_t sprite_count;              // +12
    uint32_t collider_count;            // +16
    uint32_t padding;                   // +20 (alignment)
    
    const TransformPacket* transforms;  // +24
    const SpritePacket* sprites;        // +32
    const ColliderPacket* colliders;    // +40
    
    RenderPacket() = default;
    
    RenderPacket(uint32_t frame, float dt, uint32_t t_count, uint32_t s_count, uint32_t c_count,
                 const TransformPacket* t, const SpritePacket* s, const ColliderPacket* c)
        : frame_number(frame), delta_time(dt), 
          transform_count(t_count), sprite_count(s_count), collider_count(c_count),
          padding(0), transforms(t), sprites(s), colliders(c) {}
};

#endif // MANTLE_RENDER_PACKET_H
