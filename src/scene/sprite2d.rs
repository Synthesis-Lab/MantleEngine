use glam::Vec2;
use crate::components::Sprite;
use super::node2d::Node2D;

/// Sprite2D — A 2D node with texture rendering
/// Extends Node2D, manages Sprite component for ECS rendering
#[derive(Clone, Debug)]
pub struct Sprite2D {
    /// Base 2D node with transform
    node2d: Node2D,
    /// Texture ID for rendering
    texture_id: Option<u32>,
    /// Sprite width in pixels
    width: u32,
    /// Sprite height in pixels
    height: u32,
    /// Tint color (RGBA)
    color: u32,
    /// UV mapping (min normalized coordinates)
    uv_min: Vec2,
    /// UV mapping (max normalized coordinates)
    uv_max: Vec2,
}

impl Sprite2D {
    /// Create a new Sprite2D
    pub fn new(name: impl Into<String>) -> Self {
        Sprite2D {
            node2d: Node2D::new(name),
            texture_id: None,
            width: 64,
            height: 64,
            color: 0xFFFFFFFF, // white + full alpha
            uv_min: Vec2::ZERO,
            uv_max: Vec2::ONE,
        }
    }

    /// Get the underlying Node2D
    pub fn node2d(&self) -> &Node2D {
        &self.node2d
    }

    /// Get mutable underlying Node2D
    pub fn node2d_mut(&mut self) -> &mut Node2D {
        &mut self.node2d
    }

    /// Set the texture ID
    pub fn set_texture(&mut self, texture_id: u32) -> &mut Self {
        self.texture_id = Some(texture_id);
        self
    }

    /// Get texture ID
    pub fn texture_id(&self) -> Option<u32> {
        self.texture_id
    }

    /// Clear texture
    pub fn clear_texture(&mut self) {
        self.texture_id = None;
    }

    /// Set sprite dimensions
    pub fn set_size(&mut self, width: u32, height: u32) -> &mut Self {
        self.width = width;
        self.height = height;
        self
    }

    /// Get width
    pub fn width(&self) -> u32 {
        self.width
    }

    /// Get height
    pub fn height(&self) -> u32 {
        self.height
    }

    /// Set color (RGBA format: 0xRRGGBBAA)
    pub fn set_color(&mut self, color: u32) -> &mut Self {
        self.color = color;
        self
    }

    /// Get color
    pub fn color(&self) -> u32 {
        self.color
    }

    /// Set UV rect (normalized 0.0-1.0)
    pub fn set_uv_rect(&mut self, min: Vec2, max: Vec2) -> &mut Self {
        self.uv_min = min;
        self.uv_max = max;
        self
    }

    /// Get UV min
    pub fn uv_min(&self) -> Vec2 {
        self.uv_min
    }

    /// Get UV max
    pub fn uv_max(&self) -> Vec2 {
        self.uv_max
    }

    /// Reset UV to full texture (0,0) to (1,1)
    pub fn reset_uv(&mut self) {
        self.uv_min = Vec2::ZERO;
        self.uv_max = Vec2::ONE;
    }

    /// Create a Sprite component for ECS
    pub fn to_sprite(&self) -> Sprite {
        let texture_id = self
            .texture_id
            .map(crate::assets::TextureId::new)
            .unwrap_or_else(|| crate::assets::TextureId::new(0));
        
        let mut sprite = Sprite::new(texture_id, self.width as f32, self.height as f32);
        sprite.set_color(self.color);
        sprite.set_uv(self.uv_min, self.uv_max);
        sprite
    }

    /// String representation
    pub fn to_string(&self) -> String {
        let texture_str = self
            .texture_id
            .map(|id| format!("texture_id={}", id))
            .unwrap_or_else(|| "no texture".to_string());
        format!(
            "Sprite2D(name={}, size={}x{}, color=0x{:08X}, {})",
            self.node2d.name(),
            self.width,
            self.height,
            self.color,
            texture_str
        )
    }
}

impl Default for Sprite2D {
    fn default() -> Self {
        Self::new("Sprite2D")
    }
}

// Convenience getters for Node2D properties
impl Sprite2D {
    pub fn position(&self) -> Vec2 {
        self.node2d.position()
    }

    pub fn set_position(&mut self, position: Vec2) -> &mut Self {
        self.node2d_mut().set_position(position);
        self
    }

    pub fn x(&self) -> f32 {
        self.node2d.x()
    }

    pub fn set_x(&mut self, x: f32) -> &mut Self {
        self.node2d_mut().set_x(x);
        self
    }

    pub fn y(&self) -> f32 {
        self.node2d.y()
    }

    pub fn set_y(&mut self, y: f32) -> &mut Self {
        self.node2d_mut().set_y(y);
        self
    }

    pub fn rotation_degrees(&self) -> f32 {
        self.node2d.rotation_degrees()
    }

    pub fn set_rotation_degrees(&mut self, degrees: f32) -> &mut Self {
        self.node2d_mut().set_rotation_degrees(degrees);
        self
    }

    pub fn scale(&self) -> Vec2 {
        self.node2d.scale()
    }

    pub fn set_scale(&mut self, scale: Vec2) -> &mut Self {
        self.node2d_mut().set_scale(scale);
        self
    }

    pub fn z_index(&self) -> i32 {
        self.node2d.z_index()
    }

    pub fn set_z_index(&mut self, z: i32) -> &mut Self {
        self.node2d_mut().set_z_index(z);
        self
    }

    pub fn translate(&mut self, offset: Vec2) -> &mut Self {
        self.node2d_mut().translate(offset);
        self
    }

    pub fn rotate(&mut self, degrees: f32) -> &mut Self {
        self.node2d_mut().rotate(degrees);
        self
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_sprite2d_creation() {
        let sprite = Sprite2D::new("Player");
        assert_eq!(sprite.node2d().name(), "Player");
        assert_eq!(sprite.width(), 64);
        assert_eq!(sprite.height(), 64);
        assert_eq!(sprite.color(), 0xFFFFFFFF);
        assert!(sprite.texture_id().is_none());
    }

    #[test]
    fn test_sprite2d_set_texture() {
        let mut sprite = Sprite2D::new("Sprite");
        sprite.set_texture(42);
        assert_eq!(sprite.texture_id(), Some(42));
    }

    #[test]
    fn test_sprite2d_clear_texture() {
        let mut sprite = Sprite2D::new("Sprite");
        sprite.set_texture(100);
        assert_eq!(sprite.texture_id(), Some(100));
        sprite.clear_texture();
        assert!(sprite.texture_id().is_none());
    }

    #[test]
    fn test_sprite2d_set_size() {
        let mut sprite = Sprite2D::new("Sprite");
        sprite.set_size(128, 96);
        assert_eq!(sprite.width(), 128);
        assert_eq!(sprite.height(), 96);
    }

    #[test]
    fn test_sprite2d_color() {
        let mut sprite = Sprite2D::new("Sprite");
        sprite.set_color(0xFF0000FF); // red
        assert_eq!(sprite.color(), 0xFF0000FF);
    }

    #[test]
    fn test_sprite2d_uv_rect() {
        let mut sprite = Sprite2D::new("Sprite");
        sprite.set_uv_rect(Vec2::new(0.25, 0.25), Vec2::new(0.75, 0.75));
        assert_eq!(sprite.uv_min(), Vec2::new(0.25, 0.25));
        assert_eq!(sprite.uv_max(), Vec2::new(0.75, 0.75));
    }

    #[test]
    fn test_sprite2d_reset_uv() {
        let mut sprite = Sprite2D::new("Sprite");
        sprite.set_uv_rect(Vec2::new(0.25, 0.25), Vec2::new(0.75, 0.75));
        sprite.reset_uv();
        assert_eq!(sprite.uv_min(), Vec2::ZERO);
        assert_eq!(sprite.uv_max(), Vec2::ONE);
    }

    #[test]
    fn test_sprite2d_to_sprite() {
        let mut sprite = Sprite2D::new("Test");
        sprite.set_size(100, 100);
        sprite.set_color(0xFF00FF00); // green
        sprite.set_uv_rect(Vec2::new(0.1, 0.2), Vec2::new(0.9, 0.8));

        let ecs_sprite = sprite.to_sprite();
        assert_eq!(ecs_sprite.width(), 100.0);
        assert_eq!(ecs_sprite.height(), 100.0);
    }

    #[test]
    fn test_sprite2d_position_forwarding() {
        let mut sprite = Sprite2D::new("Sprite");
        sprite.set_position(Vec2::new(10.0, 20.0));
        assert_eq!(sprite.position(), Vec2::new(10.0, 20.0));
        assert_eq!(sprite.x(), 10.0);
        assert_eq!(sprite.y(), 20.0);
    }

    #[test]
    fn test_sprite2d_rotation_forwarding() {
        let mut sprite = Sprite2D::new("Sprite");
        sprite.set_rotation_degrees(45.0);
        assert_eq!(sprite.rotation_degrees(), 45.0);
    }

    #[test]
    fn test_sprite2d_scale_forwarding() {
        let mut sprite = Sprite2D::new("Sprite");
        sprite.set_scale(Vec2::new(2.0, 2.0));
        assert_eq!(sprite.scale(), Vec2::new(2.0, 2.0));
    }

    #[test]
    fn test_sprite2d_z_index_forwarding() {
        let mut sprite = Sprite2D::new("Sprite");
        sprite.set_z_index(5);
        assert_eq!(sprite.z_index(), 5);
    }

    #[test]
    fn test_sprite2d_method_chaining() {
        let mut sprite = Sprite2D::new("Test");
        sprite
            .set_position(Vec2::new(5.0, 10.0))
            .set_size(32, 32)
            .set_color(0x00FF00FF)
            .set_z_index(10);

        assert_eq!(sprite.position(), Vec2::new(5.0, 10.0));
        assert_eq!(sprite.width(), 32);
        assert_eq!(sprite.height(), 32);
        assert_eq!(sprite.color(), 0x00FF00FF);
        assert_eq!(sprite.z_index(), 10);
    }

    #[test]
    fn test_sprite2d_to_string() {
        let mut sprite = Sprite2D::new("PlayerSprite");
        sprite.set_texture(7);
        let s = sprite.to_string();
        assert!(s.contains("PlayerSprite"));
        assert!(s.contains("texture_id=7"));
    }
}
