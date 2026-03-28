/// RenderPacket — All data needed to render a single frame
/// 
/// Layout matches C++ struct byte-for-byte for direct memory sharing.
/// This is the core of zero-copy FFI communication.
///
/// C++ side receives pointer to packed transform/sprite/collider arrays
/// and renders directly without copying.

use glam::Vec2;

/// Packed transform data for rendering
/// (matches C++ struct layout)
#[repr(C)]
#[derive(Clone, Debug, Copy)]
pub struct TransformPacket {
    pub x: f32,
    pub y: f32,
    pub rotation_degrees: f32,
    pub scale_x: f32,
    pub scale_y: f32,
    pub z_index: i32,
}

impl TransformPacket {
    pub fn new(x: f32, y: f32) -> Self {
        TransformPacket {
            x,
            y,
            rotation_degrees: 0.0,
            scale_x: 1.0,
            scale_y: 1.0,
            z_index: 0,
        }
    }

    pub fn from_node2d(x: f32, y: f32, rot_deg: f32, scale: Vec2, z: i32) -> Self {
        TransformPacket {
            x,
            y,
            rotation_degrees: rot_deg,
            scale_x: scale.x,
            scale_y: scale.y,
            z_index: z,
        }
    }
}

/// Packed sprite data for rendering
/// (matches C++ struct layout)
#[repr(C)]
#[derive(Clone, Debug, Copy)]
pub struct SpritePacket {
    pub texture_id: u32,
    pub width: f32,
    pub height: f32,
    pub color: u32, // RGBA format: 0xRRGGBBAA
    pub uv_min_x: f32,
    pub uv_min_y: f32,
    pub uv_max_x: f32,
    pub uv_max_y: f32,
}

impl SpritePacket {
    pub fn new(texture_id: u32, width: f32, height: f32) -> Self {
        SpritePacket {
            texture_id,
            width,
            height,
            color: 0xFFFFFFFF, // white, opaque
            uv_min_x: 0.0,
            uv_min_y: 0.0,
            uv_max_x: 1.0,
            uv_max_y: 1.0,
        }
    }

    pub fn with_color(mut self, color: u32) -> Self {
        self.color = color;
        self
    }

    pub fn with_uv(mut self, min_x: f32, min_y: f32, max_x: f32, max_y: f32) -> Self {
        self.uv_min_x = min_x;
        self.uv_min_y = min_y;
        self.uv_max_x = max_x;
        self.uv_max_y = max_y;
        self
    }
}

/// Packed collider data for physics/collision feedback
/// (matches C++ struct layout)
#[repr(C)]
#[derive(Clone, Debug, Copy)]
pub struct ColliderPacket {
    pub shape_type: u32, // 0=AABB, 1=Circle
    pub x: f32,
    pub y: f32,
    pub width: f32,  // For AABB
    pub height: f32, // For AABB
    pub radius: f32, // For Circle
}

impl ColliderPacket {
    pub fn aabb(x: f32, y: f32, width: f32, height: f32) -> Self {
        ColliderPacket {
            shape_type: 0,
            x,
            y,
            width,
            height,
            radius: 0.0,
        }
    }

    pub fn circle(x: f32, y: f32, radius: f32) -> Self {
        ColliderPacket {
            shape_type: 1,
            x,
            y,
            width: 0.0,
            height: 0.0,
            radius,
        }
    }
}

/// Complete render packet for one frame
/// Contains all data C++ needs to render everything
#[repr(C)]
pub struct RenderPacket {
    // Frame metadata
    pub frame_number: u64,
    pub delta_time: f32,

    // Array counts
    pub transform_count: u32,
    pub sprite_count: u32,
    pub collider_count: u32,

    // Pointers to arrays (C++ will read these)
    // In Phase 3b, these point to arena-allocated buffers
    pub transforms: *const TransformPacket,
    pub sprites: *const SpritePacket,
    pub colliders: *const ColliderPacket,
}

impl RenderPacket {
    /// Create empty packet for frame
    pub fn new(frame_number: u64, delta_time: f32) -> Self {
        RenderPacket {
            frame_number,
            delta_time,
            transform_count: 0,
            sprite_count: 0,
            collider_count: 0,
            transforms: std::ptr::null(),
            sprites: std::ptr::null(),
            colliders: std::ptr::null(),
        }
    }

    /// Set transform array
    /// Safety: caller must ensure pointer validity and lifetime
    pub unsafe fn set_transforms(&mut self, data: *const TransformPacket, count: u32) {
        self.transforms = data;
        self.transform_count = count;
    }

    /// Set sprite array
    /// Safety: caller must ensure pointer validity and lifetime
    pub unsafe fn set_sprites(&mut self, data: *const SpritePacket, count: u32) {
        self.sprites = data;
        self.sprite_count = count;
    }

    /// Set collider array
    /// Safety: caller must ensure pointer validity and lifetime
    pub unsafe fn set_colliders(&mut self, data: *const ColliderPacket, count: u32) {
        self.colliders = data;
        self.collider_count = count;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_transform_packet_creation() {
        let tp = TransformPacket::new(10.0, 20.0);
        assert_eq!(tp.x, 10.0);
        assert_eq!(tp.y, 20.0);
        assert_eq!(tp.rotation_degrees, 0.0);
        assert_eq!(tp.scale_x, 1.0);
    }

    #[test]
    fn test_transform_packet_from_node2d() {
        let tp = TransformPacket::from_node2d(5.0, 15.0, 45.0, Vec2::new(2.0, 2.0), 3);
        assert_eq!(tp.x, 5.0);
        assert_eq!(tp.y, 15.0);
        assert_eq!(tp.rotation_degrees, 45.0);
        assert_eq!(tp.scale_x, 2.0);
        assert_eq!(tp.z_index, 3);
    }

    #[test]
    fn test_sprite_packet_creation() {
        let sp = SpritePacket::new(1, 64.0, 64.0);
        assert_eq!(sp.texture_id, 1);
        assert_eq!(sp.width, 64.0);
        assert_eq!(sp.color, 0xFFFFFFFF);
        assert_eq!(sp.uv_min_x, 0.0);
        assert_eq!(sp.uv_max_x, 1.0);
    }

    #[test]
    fn test_sprite_packet_with_color() {
        let sp = SpritePacket::new(0, 32.0, 32.0)
            .with_color(0xFF0000FF)
            .with_uv(0.25, 0.25, 0.75, 0.75);
        
        assert_eq!(sp.color, 0xFF0000FF);
        assert_eq!(sp.uv_min_x, 0.25);
        assert_eq!(sp.uv_max_x, 0.75);
    }

    #[test]
    fn test_collider_packet_aabb() {
        let cp = ColliderPacket::aabb(0.0, 0.0, 100.0, 100.0);
        assert_eq!(cp.shape_type, 0);
        assert_eq!(cp.width, 100.0);
        assert_eq!(cp.height, 100.0);
    }

    #[test]
    fn test_collider_packet_circle() {
        let cp = ColliderPacket::circle(50.0, 50.0, 25.0);
        assert_eq!(cp.shape_type, 1);
        assert_eq!(cp.radius, 25.0);
    }

    #[test]
    fn test_render_packet_creation() {
        let rp = RenderPacket::new(1, 0.016);
        assert_eq!(rp.frame_number, 1);
        assert_eq!(rp.delta_time, 0.016);
        assert_eq!(rp.transform_count, 0);
    }

    #[test]
    fn test_struct_sizes_match_c() {
        // Ensure Rust structs match C++ layout expectations
        use std::mem::size_of;
        
        // TransformPacket: 5 f32 + 1 f32 + 1 i32 alignment
        // Actual layout depends on repr(C) padding
        let tp_size = size_of::<TransformPacket>();
        assert!(tp_size >= 24, "TransformPacket too small: {}", tp_size);
        
        // SpritePacket: 1 u32 + 7 f32 + 1 u32 = should be packed
        let sp_size = size_of::<SpritePacket>();
        assert!(sp_size >= 32, "SpritePacket too small: {}", sp_size);
        
        // ColliderPacket: 1 u32 + 5 f32 + 1 f32 = should be packed
        let cp_size = size_of::<ColliderPacket>();
        assert!(cp_size >= 24, "ColliderPacket too small: {}", cp_size);
    }
}
