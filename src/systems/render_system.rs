//! RenderSystem - Phase 5 Rendering Pipeline
//!
//! Responsible for:
//! 1. Staging render data (transforms, sprites, colliders)
//! 2. Building zero-copy render packets for C++ Vulkan renderer
//! 3. Managing frame synchronization
//!
//! Phase 5 Flow:
//! ```text
//! Game Loop
//!   ├─ Collect data into RenderData
//!   ├─ Call build_render_packet()
//!   └─ Submit to C++ via FFI
//! ```

use crate::ffi::render_packet::{ColliderPacket, RenderPacket, SpritePacket, TransformPacket};

/// RenderData - Staging container for rendering primitives
/// 
/// In full implementation, these come from Scene traversal or ECS queries.
/// For Phase 5, we support manual addition of packets.
#[derive(Clone)]
pub struct RenderData {
    /// Transform packets (position, rotation, scale, z-index)
    pub transform_packets: Vec<TransformPacket>,
    /// Sprite packets (texture, color, UV)
    pub sprite_packets: Vec<SpritePacket>,
    /// Collider packets (debug rendering)
    pub collider_packets: Vec<ColliderPacket>,
    /// Frame metadata
    pub frame_number: u64,
    pub delta_time: f32,
}

impl RenderData {
    /// Create empty render data for frame
    pub fn new(frame_number: u64, delta_time: f32) -> Self {
        Self {
            transform_packets: Vec::new(),
            sprite_packets: Vec::new(),
            collider_packets: Vec::new(),
            frame_number,
            delta_time,
        }
    }

    /// Clear all packets (prepare for next frame)
    pub fn clear(&mut self) {
        self.transform_packets.clear();
        self.sprite_packets.clear();
        self.collider_packets.clear();
    }

    /// Add a transform packet
    pub fn add_transform(&mut self, packet: TransformPacket) {
        self.transform_packets.push(packet);
    }

    /// Add multiple transform packets
    pub fn add_transforms(&mut self, packets: Vec<TransformPacket>) {
        self.transform_packets.extend(packets);
    }

    /// Add a sprite packet
    pub fn add_sprite(&mut self, packet: SpritePacket) {
        self.sprite_packets.push(packet);
    }

    /// Add multiple sprite packets
    pub fn add_sprites(&mut self, packets: Vec<SpritePacket>) {
        self.sprite_packets.extend(packets);
    }

    /// Add a collider packet (debug rendering)
    pub fn add_collider(&mut self, packet: ColliderPacket) {
        self.collider_packets.push(packet);
    }

    /// Add multiple collider packets
    pub fn add_colliders(&mut self, packets: Vec<ColliderPacket>) {
        self.collider_packets.extend(packets);
    }

    /// Get total packet count across all types
    pub fn total_packets(&self) -> usize {
        self.transform_packets.len() + self.sprite_packets.len() + self.collider_packets.len()
    }

    /// Estimate memory usage in bytes
    pub fn estimated_size_bytes(&self) -> usize {
        (self.transform_packets.len() * 28) // TransformPacket is 28 bytes
            + (self.sprite_packets.len() * 36) // SpritePacket is 36 bytes
            + (self.collider_packets.len() * 24) // ColliderPacket is 24 bytes
    }
}

impl Default for RenderData {
    fn default() -> Self {
        Self::new(0, 0.0)
    }
}

/// RenderSystem - Manages frame data and packet building
pub struct RenderSystem {
    /// Current frame staging area
    current_frame: RenderData,
    /// Total frames submitted
    total_frames_submitted: u64,
}

impl RenderSystem {
    /// Create new render system
    pub fn new() -> Self {
        Self {
            current_frame: RenderData::new(0, 0.0),
            total_frames_submitted: 0,
        }
    }

    /// Get mutable reference to current frame data
    pub fn current_frame_mut(&mut self) -> &mut RenderData {
        &mut self.current_frame
    }

    /// Get read-only reference to current frame data
    pub fn current_frame(&self) -> &RenderData {
        &self.current_frame
    }

    /// Build render packet from current frame data
    /// 
    /// Creates zero-copy FFI structure with pointers to internal vectors
    /// Safety: Pointers remain valid as long as RenderData is not dropped
    pub fn build_render_packet(&self) -> RenderPacket {
        RenderPacket {
            frame_number: self.current_frame.frame_number,
            delta_time: self.current_frame.delta_time,
            transform_count: self.current_frame.transform_packets.len() as u32,
            sprite_count: self.current_frame.sprite_packets.len() as u32,
            collider_count: self.current_frame.collider_packets.len() as u32,
            transforms: self.current_frame.transform_packets.as_ptr(),
            sprites: self.current_frame.sprite_packets.as_ptr(),
            colliders: self.current_frame.collider_packets.as_ptr(),
        }
    }

    /// Begin new frame (clear previous data, assign frame number)
    pub fn begin_frame(&mut self, frame_number: u64, delta_time: f32) {
        self.current_frame = RenderData::new(frame_number, delta_time);
    }

    /// Finish frame and prepare for submission
    /// 
    /// In full implementation, would submit to C++ via FFI
    pub fn submit_frame(&mut self) -> RenderStats {
        let stats = RenderStats {
            frame_number: self.current_frame.frame_number,
            transform_count: self.current_frame.transform_packets.len(),
            sprite_count: self.current_frame.sprite_packets.len(),
            collider_count: self.current_frame.collider_packets.len(),
            memory_usage: self.current_frame.estimated_size_bytes(),
        };

        self.total_frames_submitted += 1;
        stats
    }

    /// Get statistics about last submitted frame
    pub fn last_frame_stats(&self) -> (u64, usize) {
        (self.total_frames_submitted, self.current_frame.total_packets())
    }

    /// Reset system (useful for shutdown/restart)
    pub fn reset(&mut self) {
        self.current_frame.clear();
        self.total_frames_submitted = 0;
    }

    /// Get current frame render statistics
    pub fn stats(&self) -> RenderStats {
        RenderStats {
            frame_number: self.current_frame.frame_number,
            transform_count: self.current_frame.transform_packets.len(),
            sprite_count: self.current_frame.sprite_packets.len(),
            collider_count: self.current_frame.collider_packets.len(),
            memory_usage: self.current_frame.estimated_size_bytes(),
        }
    }

    /// Convenience method to add a transform from simple parameters
    pub fn add_transform_simple(&mut self, _id: u64, x: f32, y: f32) {
        let packet = TransformPacket {
            x,
            y,
            rotation_degrees: 0.0,
            scale_x: 1.0,
            scale_y: 1.0,
            z_index: 0,
        };
        self.current_frame.add_transform(packet);
    }

    /// Convenience method to add a sprite from simple parameters
    pub fn add_sprite_simple(&mut self, _id: u64, _texture_name: &str) {
        let packet = SpritePacket::new(1, 64.0, 64.0);
        self.current_frame.add_sprite(packet);
    }

    /// Convenience method to add a collider from simple parameters
    pub fn add_collider_simple(&mut self, _id: u64, width: f32, height: f32) {
        let packet = ColliderPacket::aabb(0.0, 0.0, width, height);
        self.current_frame.add_collider(packet);
    }
}

impl Default for RenderSystem {
    fn default() -> Self {
        Self::new()
    }
}

/// Statistics about render frame
#[derive(Debug, Clone, Default)]
pub struct RenderStats {
    pub frame_number: u64,
    pub transform_count: usize,
    pub sprite_count: usize,
    pub collider_count: usize,
    pub memory_usage: usize,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_render_system_creation() {
        let system = RenderSystem::new();
        assert_eq!(system.total_frames_submitted, 0);
    }

    #[test]
    fn test_render_data_new() {
        let data = RenderData::new(1, 0.016);
        assert_eq!(data.frame_number, 1);
        assert_eq!(data.delta_time, 0.016);
        assert_eq!(data.total_packets(), 0);
    }

    #[test]
    fn test_render_data_add_transforms() {
        let mut data = RenderData::new(0, 0.016);
        data.add_transform(TransformPacket {
            x: 10.0,
            y: 20.0,
            rotation_degrees: 45.0,
            scale_x: 1.0,
            scale_y: 1.0,
            z_index: 0,
        });

        assert_eq!(data.transform_packets.len(), 1);
        assert_eq!(data.total_packets(), 1);
    }

    #[test]
    fn test_render_data_add_sprites() {
        let mut data = RenderData::new(0, 0.016);
        let sprite = SpritePacket::new(1, 64.0, 64.0);
        data.add_sprite(sprite);

        assert_eq!(data.sprite_packets.len(), 1);
        assert_eq!(data.total_packets(), 1);
    }

    #[test]
    fn test_render_data_add_colliders() {
        let mut data = RenderData::new(0, 0.016);
        let collider = ColliderPacket::aabb(0.0, 0.0, 64.0, 64.0);
        data.add_collider(collider);

        assert_eq!(data.collider_packets.len(), 1);
        assert_eq!(data.total_packets(), 1);
    }

    #[test]
    fn test_render_data_clear() {
        let mut data = RenderData::new(0, 0.016);
        data.add_transform(TransformPacket {
            x: 0.0,
            y: 0.0,
            rotation_degrees: 0.0,
            scale_x: 1.0,
            scale_y: 1.0,
            z_index: 0,
        });
        data.add_sprite(SpritePacket::new(1, 64.0, 64.0));

        assert_eq!(data.total_packets(), 2);
        data.clear();
        assert_eq!(data.total_packets(), 0);
    }

    #[test]
    fn test_render_data_estimated_size() {
        let mut data = RenderData::new(0, 0.016);
        data.add_transform(TransformPacket {
            x: 0.0,
            y: 0.0,
            rotation_degrees: 0.0,
            scale_x: 1.0,
            scale_y: 1.0,
            z_index: 0,
        });
        data.add_sprite(SpritePacket::new(1, 64.0, 64.0));
        data.add_collider(ColliderPacket::aabb(0.0, 0.0, 64.0, 64.0));

        // 28 + 36 + 24 = 88 bytes
        assert_eq!(data.estimated_size_bytes(), 88);
    }

    #[test]
    fn test_render_system_begin_frame() {
        let mut system = RenderSystem::new();
        system.begin_frame(42, 0.016);

        assert_eq!(system.current_frame.frame_number, 42);
        assert_eq!(system.current_frame.delta_time, 0.016);
    }

    #[test]
    fn test_render_system_build_packet() {
        let mut system = RenderSystem::new();
        system.begin_frame(1, 0.016);

        system.current_frame_mut().add_transform(TransformPacket {
            x: 0.0,
            y: 0.0,
            rotation_degrees: 0.0,
            scale_x: 1.0,
            scale_y: 1.0,
            z_index: 0,
        });

        let packet = system.build_render_packet();
        assert_eq!(packet.frame_number, 1);
        assert_eq!(packet.transform_count, 1);
    }

    #[test]
    fn test_render_system_submit_frame() {
        let mut system = RenderSystem::new();
        system.begin_frame(1, 0.016);

        system.current_frame_mut().add_transform(TransformPacket {
            x: 0.0,
            y: 0.0,
            rotation_degrees: 0.0,
            scale_x: 1.0,
            scale_y: 1.0,
            z_index: 0,
        });

        let stats = system.submit_frame();
        assert_eq!(stats.frame_number, 1);
        assert_eq!(stats.transform_count, 1);
        assert_eq!(system.total_frames_submitted, 1);
    }
}

