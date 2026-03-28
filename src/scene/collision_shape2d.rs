use glam::Vec2;
use crate::components::Collider;
use super::node2d::Node2D;

/// CollisionShape2D — Defines collision shape for a node
/// Integrates with Bevy Collider component for physics
#[derive(Clone, Debug)]
pub struct CollisionShape2D {
    /// Base 2D node
    node2d: Node2D,
    /// Collision shape type
    shape: CollisionShape,
}

/// Collision shape types
#[derive(Clone, Debug)]
pub enum CollisionShape {
    /// Axis-aligned bounding box: (width, height)
    AABB(f32, f32),
    /// Circle: radius
    Circle(f32),
}

impl CollisionShape2D {
    /// Create a new CollisionShape2D with AABB
    pub fn new_aabb(name: impl Into<String>, width: f32, height: f32) -> Self {
        CollisionShape2D {
            node2d: Node2D::new(name),
            shape: CollisionShape::AABB(width, height),
        }
    }

    /// Create a new CollisionShape2D with Circle
    pub fn new_circle(name: impl Into<String>, radius: f32) -> Self {
        CollisionShape2D {
            node2d: Node2D::new(name),
            shape: CollisionShape::Circle(radius),
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

    /// Get collision shape
    pub fn shape(&self) -> &CollisionShape {
        &self.shape
    }

    /// Get mutable collision shape
    pub fn shape_mut(&mut self) -> &mut CollisionShape {
        &mut self.shape
    }

    /// Set as AABB
    pub fn set_aabb(&mut self, width: f32, height: f32) -> &mut Self {
        self.shape = CollisionShape::AABB(width, height);
        self
    }

    /// Set as Circle
    pub fn set_circle(&mut self, radius: f32) -> &mut Self {
        self.shape = CollisionShape::Circle(radius);
        self
    }

    /// Check if shape is AABB
    pub fn is_aabb(&self) -> bool {
        matches!(self.shape, CollisionShape::AABB(_, _))
    }

    /// Check if shape is Circle
    pub fn is_circle(&self) -> bool {
        matches!(self.shape, CollisionShape::Circle(_))
    }

    /// Get AABB dimensions if applicable
    pub fn aabb_dimensions(&self) -> Option<(f32, f32)> {
        match self.shape {
            CollisionShape::AABB(w, h) => Some((w, h)),
            _ => None,
        }
    }

    /// Get circle radius if applicable
    pub fn circle_radius(&self) -> Option<f32> {
        match self.shape {
            CollisionShape::Circle(r) => Some(r),
            _ => None,
        }
    }

    /// Create a Collider component for ECS
    pub fn to_collider(&self) -> Collider {
        match self.shape {
            CollisionShape::AABB(width, height) => {
                Collider::aabb(self.node2d.x(), self.node2d.y(), width, height)
            }
            CollisionShape::Circle(radius) => {
                Collider::circle(self.node2d.x(), self.node2d.y(), radius)
            }
        }
    }

    /// String representation
    pub fn to_string(&self) -> String {
        let shape_str = match self.shape {
            CollisionShape::AABB(w, h) => format!("AABB({}x{})", w, h),
            CollisionShape::Circle(r) => format!("Circle(r={})", r),
        };
        format!(
            "CollisionShape2D(name={}, shape={}, pos=[{:.1}, {:.1}])",
            self.node2d.name(),
            shape_str,
            self.node2d.x(),
            self.node2d.y()
        )
    }
}

impl Default for CollisionShape2D {
    fn default() -> Self {
        Self::new_aabb("CollisionShape2D", 32.0, 32.0)
    }
}

// Convenience getters for Node2D properties
impl CollisionShape2D {
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
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_collision_shape2d_creation_aabb() {
        let collision = CollisionShape2D::new_aabb("WallCollider", 100.0, 50.0);
        assert_eq!(collision.node2d().name(), "WallCollider");
        assert!(collision.is_aabb());
        assert!(!collision.is_circle());
        assert_eq!(collision.aabb_dimensions(), Some((100.0, 50.0)));
    }

    #[test]
    fn test_collision_shape2d_creation_circle() {
        let collision = CollisionShape2D::new_circle("BulletCollider", 5.0);
        assert_eq!(collision.node2d().name(), "BulletCollider");
        assert!(!collision.is_aabb());
        assert!(collision.is_circle());
        assert_eq!(collision.circle_radius(), Some(5.0));
    }

    #[test]
    fn test_collision_shape2d_set_aabb() {
        let mut collision = CollisionShape2D::new_circle("Test", 10.0);
        collision.set_aabb(64.0, 32.0);
        assert!(collision.is_aabb());
        assert_eq!(collision.aabb_dimensions(), Some((64.0, 32.0)));
    }

    #[test]
    fn test_collision_shape2d_set_circle() {
        let mut collision = CollisionShape2D::new_aabb("Test", 64.0, 64.0);
        collision.set_circle(15.0);
        assert!(collision.is_circle());
        assert_eq!(collision.circle_radius(), Some(15.0));
    }

    #[test]
    fn test_collision_shape2d_aabb_dimensions() {
        let collision = CollisionShape2D::new_aabb("Test", 50.0, 75.0);
        assert_eq!(collision.aabb_dimensions(), Some((50.0, 75.0)));
        assert_eq!(collision.circle_radius(), None);
    }

    #[test]
    fn test_collision_shape2d_circle_radius() {
        let collision = CollisionShape2D::new_circle("Test", 20.0);
        assert_eq!(collision.circle_radius(), Some(20.0));
        assert_eq!(collision.aabb_dimensions(), None);
    }

    #[test]
    fn test_collision_shape2d_position() {
        let mut collision = CollisionShape2D::new_aabb("Test", 32.0, 32.0);
        collision.set_position(Vec2::new(100.0, 200.0));
        assert_eq!(collision.position(), Vec2::new(100.0, 200.0));
        assert_eq!(collision.x(), 100.0);
        assert_eq!(collision.y(), 200.0);
    }

    #[test]
    fn test_collision_shape2d_to_collider_aabb() {
        let mut collision = CollisionShape2D::new_aabb("Test", 64.0, 32.0);
        collision.set_position(Vec2::new(10.0, 20.0));
        let collider = collision.to_collider();

        // Verify it's an AABB collider
        match collider {
            Collider::Aabb {
                x,
                y,
                width,
                height,
            } => {
                assert_eq!(x, 10.0);
                assert_eq!(y, 20.0);
                assert_eq!(width, 64.0);
                assert_eq!(height, 32.0);
            }
            _ => panic!("Expected Aabb collider"),
        }
    }

    #[test]
    fn test_collision_shape2d_to_collider_circle() {
        let mut collision = CollisionShape2D::new_circle("Test", 25.0);
        collision.set_position(Vec2::new(50.0, 75.0));
        let collider = collision.to_collider();

        // Verify it's a Circle collider
        match collider {
            Collider::Circle { x, y, radius } => {
                assert_eq!(x, 50.0);
                assert_eq!(y, 75.0);
                assert_eq!(radius, 25.0);
            }
            _ => panic!("Expected Circle collider"),
        }
    }

    #[test]
    fn test_collision_shape2d_method_chaining() {
        let mut collision = CollisionShape2D::new_aabb("Test", 32.0, 32.0);
        collision
            .set_position(Vec2::new(10.0, 20.0))
            .set_z_index(5)
            .set_aabb(48.0, 48.0);

        assert_eq!(collision.position(), Vec2::new(10.0, 20.0));
        assert_eq!(collision.z_index(), 5);
        assert_eq!(collision.aabb_dimensions(), Some((48.0, 48.0)));
    }

    #[test]
    fn test_collision_shape2d_translate() {
        let mut collision = CollisionShape2D::new_aabb("Test", 32.0, 32.0);
        collision.set_position(Vec2::new(10.0, 10.0));
        collision.translate(Vec2::new(5.0, -5.0));
        assert_eq!(collision.position(), Vec2::new(15.0, 5.0));
    }

    #[test]
    fn test_collision_shape2d_to_string() {
        let collision = CollisionShape2D::new_aabb("WallCollider", 100.0, 50.0);
        let s = collision.to_string();
        assert!(s.contains("WallCollider"));
        assert!(s.contains("AABB"));
    }
}
