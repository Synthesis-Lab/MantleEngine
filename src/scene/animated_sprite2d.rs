use super::sprite2d::Sprite2D;

/// AnimatedSprite2D — A Sprite2D that plays animations
/// Extends Sprite2D with animation playback control
#[derive(Clone, Debug)]
pub struct AnimatedSprite2D {
    /// Base sprite node
    sprite2d: Sprite2D,
    /// Current animation ID
    animation_id: Option<u32>,
    /// Current frame index
    frame_index: usize,
    /// Elapsed time in current frame
    elapsed_time: f32,
    /// Is animation currently playing?
    is_playing: bool,
    /// Animation playback speed multiplier
    speed: f32,
}

impl AnimatedSprite2D {
    /// Create a new AnimatedSprite2D
    pub fn new(name: impl Into<String>) -> Self {
        AnimatedSprite2D {
            sprite2d: Sprite2D::new(name),
            animation_id: None,
            frame_index: 0,
            elapsed_time: 0.0,
            is_playing: false,
            speed: 1.0,
        }
    }

    /// Get the underlying Sprite2D
    pub fn sprite2d(&self) -> &Sprite2D {
        &self.sprite2d
    }

    /// Get mutable underlying Sprite2D
    pub fn sprite2d_mut(&mut self) -> &mut Sprite2D {
        &mut self.sprite2d
    }

    /// Set the animation to play
    pub fn set_animation(&mut self, animation_id: u32) -> &mut Self {
        self.animation_id = Some(animation_id);
        self.frame_index = 0;
        self.elapsed_time = 0.0;
        self
    }

    /// Get current animation ID
    pub fn animation_id(&self) -> Option<u32> {
        self.animation_id
    }

    /// Start playing the animation
    pub fn play(&mut self) -> &mut Self {
        self.is_playing = true;
        self
    }

    /// Stop playing the animation
    pub fn stop(&mut self) -> &mut Self {
        self.is_playing = false;
        self.frame_index = 0;
        self.elapsed_time = 0.0;
        self
    }

    /// Pause the animation (keep current frame)
    pub fn pause(&mut self) -> &mut Self {
        self.is_playing = false;
        self
    }

    /// Resume playing
    pub fn resume(&mut self) -> &mut Self {
        self.is_playing = true;
        self
    }

    /// Check if animation is playing
    pub fn is_playing(&self) -> bool {
        self.is_playing
    }

    /// Set playback speed (1.0 = normal, 2.0 = 2x, 0.5 = half)
    pub fn set_speed(&mut self, speed: f32) -> &mut Self {
        self.speed = speed.max(0.0);
        self
    }

    /// Get playback speed
    pub fn speed(&self) -> f32 {
        self.speed
    }

    /// Get current frame index
    pub fn frame_index(&self) -> usize {
        self.frame_index
    }

    /// Get elapsed time in current frame
    pub fn elapsed_time(&self) -> f32 {
        self.elapsed_time
    }

    /// Advance animation by delta time (call from AnimatedSpriteSystem)
    pub fn process(&mut self, delta_time: f32) {
        if !self.is_playing || self.animation_id.is_none() {
            return;
        }

        self.elapsed_time += delta_time * self.speed;
        // Note: actual frame advance happens in AnimatedSpriteSystem with animation data
    }

    /// String representation
    pub fn to_string(&self) -> String {
        let anim_str = self
            .animation_id
            .map(|id| format!("animation_id={}", id))
            .unwrap_or_else(|| "no animation".to_string());
        format!(
            "AnimatedSprite2D(name={}, {}, frame={}, playing={})",
            self.sprite2d.node2d().name(),
            anim_str,
            self.frame_index,
            self.is_playing
        )
    }
}

impl Default for AnimatedSprite2D {
    fn default() -> Self {
        Self::new("AnimatedSprite2D")
    }
}

// Convenience getters for Sprite2D properties
impl AnimatedSprite2D {
    pub fn set_texture(&mut self, texture_id: u32) -> &mut Self {
        self.sprite2d_mut().set_texture(texture_id);
        self
    }

    pub fn texture_id(&self) -> Option<u32> {
        self.sprite2d.texture_id()
    }

    pub fn set_size(&mut self, width: u32, height: u32) -> &mut Self {
        self.sprite2d_mut().set_size(width, height);
        self
    }

    pub fn width(&self) -> u32 {
        self.sprite2d.width()
    }

    pub fn height(&self) -> u32 {
        self.sprite2d.height()
    }

    pub fn set_color(&mut self, color: u32) -> &mut Self {
        self.sprite2d_mut().set_color(color);
        self
    }

    pub fn color(&self) -> u32 {
        self.sprite2d.color()
    }

    pub fn position(&self) -> glam::Vec2 {
        self.sprite2d.position()
    }

    pub fn set_position(&mut self, position: glam::Vec2) -> &mut Self {
        self.sprite2d_mut().set_position(position);
        self
    }

    pub fn x(&self) -> f32 {
        self.sprite2d.x()
    }

    pub fn set_x(&mut self, x: f32) -> &mut Self {
        self.sprite2d_mut().set_x(x);
        self
    }

    pub fn y(&self) -> f32 {
        self.sprite2d.y()
    }

    pub fn set_y(&mut self, y: f32) -> &mut Self {
        self.sprite2d_mut().set_y(y);
        self
    }

    pub fn rotation_degrees(&self) -> f32 {
        self.sprite2d.rotation_degrees()
    }

    pub fn set_rotation_degrees(&mut self, degrees: f32) -> &mut Self {
        self.sprite2d_mut().set_rotation_degrees(degrees);
        self
    }

    pub fn scale(&self) -> glam::Vec2 {
        self.sprite2d.scale()
    }

    pub fn set_scale(&mut self, scale: glam::Vec2) -> &mut Self {
        self.sprite2d_mut().set_scale(scale);
        self
    }

    pub fn z_index(&self) -> i32 {
        self.sprite2d.z_index()
    }

    pub fn set_z_index(&mut self, z: i32) -> &mut Self {
        self.sprite2d_mut().set_z_index(z);
        self
    }

    pub fn translate(&mut self, offset: glam::Vec2) -> &mut Self {
        self.sprite2d_mut().translate(offset);
        self
    }

    pub fn rotate(&mut self, degrees: f32) -> &mut Self {
        self.sprite2d_mut().rotate(degrees);
        self
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_animated_sprite2d_creation() {
        let sprite = AnimatedSprite2D::new("Enemy");
        assert_eq!(sprite.sprite2d().node2d().name(), "Enemy");
        assert!(sprite.animation_id().is_none());
        assert!(!sprite.is_playing());
        assert_eq!(sprite.speed(), 1.0);
    }

    #[test]
    fn test_animated_sprite2d_set_animation() {
        let mut sprite = AnimatedSprite2D::new("Sprite");
        sprite.set_animation(5);
        assert_eq!(sprite.animation_id(), Some(5));
    }

    #[test]
    fn test_animated_sprite2d_play() {
        let mut sprite = AnimatedSprite2D::new("Sprite");
        sprite.set_animation(1);
        sprite.play();
        assert!(sprite.is_playing());
    }

    #[test]
    fn test_animated_sprite2d_stop() {
        let mut sprite = AnimatedSprite2D::new("Sprite");
        sprite.set_animation(1);
        sprite.play();
        assert!(sprite.is_playing());

        sprite.stop();
        assert!(!sprite.is_playing());
    }

    #[test]
    fn test_animated_sprite2d_pause_resume() {
        let mut sprite = AnimatedSprite2D::new("Sprite");
        sprite.set_animation(1);
        sprite.play();
        assert!(sprite.is_playing());

        sprite.pause();
        assert!(!sprite.is_playing());

        sprite.resume();
        assert!(sprite.is_playing());
    }

    #[test]
    fn test_animated_sprite2d_speed() {
        let mut sprite = AnimatedSprite2D::new("Sprite");
        sprite.set_speed(2.0);
        assert_eq!(sprite.speed(), 2.0);

        sprite.set_speed(0.5);
        assert_eq!(sprite.speed(), 0.5);
    }

    #[test]
    fn test_animated_sprite2d_negative_speed() {
        let mut sprite = AnimatedSprite2D::new("Sprite");
        sprite.set_speed(-5.0);
        // Should be clamped to 0.0
        assert_eq!(sprite.speed(), 0.0);
    }

    #[test]
    fn test_animated_sprite2d_process() {
        let mut sprite = AnimatedSprite2D::new("Sprite");
        sprite.set_animation(1);
        sprite.play();
        sprite.set_speed(1.0);

        sprite.process(0.016); // 16ms frame
        assert!(sprite.elapsed_time() > 0.0);
    }

    #[test]
    fn test_animated_sprite2d_process_paused() {
        let mut sprite = AnimatedSprite2D::new("Sprite");
        sprite.set_animation(1);
        // Don't play
        let old_time = sprite.elapsed_time();
        sprite.process(0.016);
        // Should not advance if not playing
        assert_eq!(sprite.elapsed_time(), old_time);
    }

    #[test]
    fn test_animated_sprite2d_method_chaining() {
        let mut sprite = AnimatedSprite2D::new("Test");
        sprite
            .set_animation(3)
            .set_size(64, 64)
            .set_speed(1.5)
            .play();

        assert_eq!(sprite.animation_id(), Some(3));
        assert_eq!(sprite.width(), 64);
        assert_eq!(sprite.height(), 64);
        assert_eq!(sprite.speed(), 1.5);
        assert!(sprite.is_playing());
    }

    #[test]
    fn test_animated_sprite2d_to_string() {
        let mut sprite = AnimatedSprite2D::new("PlayerWalk");
        sprite.set_animation(2);
        sprite.play();
        let s = sprite.to_string();
        assert!(s.contains("PlayerWalk"));
        assert!(s.contains("animation_id=2"));
        assert!(s.contains("playing=true"));
    }
}
