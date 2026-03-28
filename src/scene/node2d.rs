use glam::Vec2;
use crate::components::Transform;
use super::node::{Node, NodeId};

/// Node2D — A 2D node with transform (position, rotation, scale)
/// Extended from Node, syncs with Bevy ECS Transform component
#[derive(Clone, Debug)]
pub struct Node2D {
    /// Base node data
    node: Node,
    /// Position in 2D space
    position: Vec2,
    /// Rotation in degrees (0-360)
    rotation_degrees: f32,
    /// Scale factors (1.0 = normal)
    scale: Vec2,
    /// Z-order (for layering)
    z_index: i32,
}

impl Node2D {
    /// Create a new Node2D
    pub fn new(name: impl Into<String>) -> Self {
        Node2D {
            node: Node::new(name),
            position: Vec2::ZERO,
            rotation_degrees: 0.0,
            scale: Vec2::ONE,
            z_index: 0,
        }
    }

    /// Get the underlying Node
    pub fn node(&self) -> &Node {
        &self.node
    }

    /// Get mutable underlying Node
    pub fn node_mut(&mut self) -> &mut Node {
        &mut self.node
    }

    /// Get position
    pub fn position(&self) -> Vec2 {
        self.position
    }

    /// Set position
    pub fn set_position(&mut self, position: Vec2) {
        self.position = position;
    }

    /// Get X position
    pub fn x(&self) -> f32 {
        self.position.x
    }

    /// Set X position
    pub fn set_x(&mut self, x: f32) {
        self.position.x = x;
    }

    /// Get Y position
    pub fn y(&self) -> f32 {
        self.position.y
    }

    /// Set Y position
    pub fn set_y(&mut self, y: f32) {
        self.position.y = y;
    }

    /// Get global position (considering parent transforms)
    pub fn global_position(&self) -> Vec2 {
        // TODO: implement by traversing parents
        self.position
    }

    /// Get rotation in degrees
    pub fn rotation_degrees(&self) -> f32 {
        self.rotation_degrees
    }

    /// Set rotation in degrees
    pub fn set_rotation_degrees(&mut self, degrees: f32) {
        self.rotation_degrees = degrees % 360.0;
    }

    /// Get rotation in radians
    pub fn rotation_radians(&self) -> f32 {
        self.rotation_degrees.to_radians()
    }

    /// Set rotation in radians
    pub fn set_rotation_radians(&mut self, radians: f32) {
        self.rotation_degrees = radians.to_degrees() % 360.0;
    }

    /// Get scale
    pub fn scale(&self) -> Vec2 {
        self.scale
    }

    /// Set scale
    pub fn set_scale(&mut self, scale: Vec2) {
        self.scale = scale;
    }

    /// Get Z-index
    pub fn z_index(&self) -> i32 {
        self.z_index
    }

    /// Set Z-index
    pub fn set_z_index(&mut self, z: i32) {
        self.z_index = z;
    }

    /// Translate (move) the node
    pub fn translate(&mut self, offset: Vec2) {
        self.position += offset;
    }

    /// Translate along vector
    pub fn translate_along(&mut self, direction: Vec2, distance: f32) {
        self.position += direction.normalize_or_zero() * distance;
    }

    /// Rotate (add to rotation)
    pub fn rotate(&mut self, degrees: f32) {
        self.rotation_degrees = (self.rotation_degrees + degrees) % 360.0;
    }

    /// Reset transform to origin
    pub fn reset_transform(&mut self) {
        self.position = Vec2::ZERO;
        self.rotation_degrees = 0.0;
        self.scale = Vec2::ONE;
        self.z_index = 0;
    }

    /// Create a Bevy Transform component from this Node2D
    /// (used for syncing with ECS)
    pub fn to_transform(&self) -> Transform {
        let mut transform = Transform::new(self.position.x, self.position.y);
        transform.set_rotation_degrees(self.rotation_degrees);
        transform.set_scale_vec(self.scale);
        transform.set_z(self.z_index as f32);
        transform
    }

    /// Update this Node2D from a Bevy Transform
    /// (used for syncing from ECS)
    pub fn from_transform(&mut self, transform: &Transform) {
        self.position = Vec2::new(transform.x(), transform.y());
        self.rotation_degrees = transform.rotation_degrees();
        self.scale = transform.scale();
        self.z_index = transform.z() as i32;
    }

    /// String representation
    pub fn to_string(&self) -> String {
        format!(
            "Node2D(name={}, pos=[{:.1}, {:.1}], rot={:.1}°, scale=[{:.1}, {:.1}], z={})",
            self.node.name(),
            self.position.x,
            self.position.y,
            self.rotation_degrees,
            self.scale.x,
            self.scale.y,
            self.z_index
        )
    }
}

impl Default for Node2D {
    fn default() -> Self {
        Self::new("Node2D")
    }
}

// Convenience getters for Node properties
impl Node2D {
    pub fn id(&self) -> NodeId {
        self.node.id()
    }

    pub fn name(&self) -> &str {
        self.node.name()
    }

    pub fn set_name(&mut self, name: impl Into<String>) {
        self.node.set_name(name);
    }

    pub fn parent(&self) -> Option<NodeId> {
        self.node.parent()
    }

    pub fn children(&self) -> &[NodeId] {
        self.node.children()
    }

    pub fn is_active(&self) -> bool {
        self.node.is_active()
    }

    pub fn set_active(&mut self, active: bool) {
        self.node.set_active(active);
    }

    pub fn is_visible(&self) -> bool {
        self.node.is_visible()
    }

    pub fn set_visible(&mut self, visible: bool) {
        self.node.set_visible(visible);
    }

    pub fn child_count(&self) -> usize {
        self.node.child_count()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_node2d_creation() {
        let node = Node2D::new("Sprite");
        assert_eq!(node.name(), "Sprite");
        assert_eq!(node.position(), Vec2::ZERO);
        assert_eq!(node.rotation_degrees(), 0.0);
        assert_eq!(node.scale(), Vec2::ONE);
        assert_eq!(node.z_index(), 0);
    }

    #[test]
    fn test_node2d_position() {
        let mut node = Node2D::new("Test");
        node.set_position(Vec2::new(10.0, 20.0));
        assert_eq!(node.x(), 10.0);
        assert_eq!(node.y(), 20.0);

        node.set_x(15.0);
        assert_eq!(node.x(), 15.0);

        node.set_y(25.0);
        assert_eq!(node.y(), 25.0);
    }

    #[test]
    fn test_node2d_rotation() {
        let mut node = Node2D::new("Test");
        node.set_rotation_degrees(45.0);
        assert_eq!(node.rotation_degrees(), 45.0);

        let radians = node.rotation_radians();
        assert!((radians - std::f32::consts::PI / 4.0).abs() < 0.001);
    }

    #[test]
    fn test_node2d_rotation_wrapping() {
        let mut node = Node2D::new("Test");
        node.set_rotation_degrees(400.0);
        assert!((node.rotation_degrees() - 40.0).abs() < 0.001);
    }

    #[test]
    fn test_node2d_scale() {
        let mut node = Node2D::new("Test");
        node.set_scale(Vec2::new(2.0, 3.0));
        assert_eq!(node.scale(), Vec2::new(2.0, 3.0));
    }

    #[test]
    fn test_node2d_translate() {
        let mut node = Node2D::new("Test");
        node.set_position(Vec2::new(10.0, 10.0));
        node.translate(Vec2::new(5.0, -5.0));
        assert_eq!(node.position(), Vec2::new(15.0, 5.0));
    }

    #[test]
    fn test_node2d_rotate() {
        let mut node = Node2D::new("Test");
        node.set_rotation_degrees(30.0);
        node.rotate(15.0);
        assert_eq!(node.rotation_degrees(), 45.0);
    }

    #[test]
    fn test_node2d_reset_transform() {
        let mut node = Node2D::new("Test");
        node.set_position(Vec2::new(100.0, 200.0));
        node.set_rotation_degrees(45.0);
        node.set_scale(Vec2::new(2.0, 3.0));
        node.set_z_index(5);

        node.reset_transform();
        assert_eq!(node.position(), Vec2::ZERO);
        assert_eq!(node.rotation_degrees(), 0.0);
        assert_eq!(node.scale(), Vec2::ONE);
        assert_eq!(node.z_index(), 0);
    }

    #[test]
    fn test_node2d_to_transform() {
        let mut node = Node2D::new("Test");
        node.set_position(Vec2::new(10.0, 20.0));
        node.set_rotation_degrees(45.0);
        node.set_scale(Vec2::new(2.0, 2.0));
        node.set_z_index(5);

        let transform = node.to_transform();
        assert_eq!(transform.x(), 10.0);
        assert_eq!(transform.y(), 20.0);
        assert_eq!(transform.rotation_degrees(), 45.0);
        assert_eq!(transform.scale(), Vec2::new(2.0, 2.0));
        assert_eq!(transform.z() as i32, 5);
    }

    #[test]
    fn test_node2d_from_transform() {
        let tf = Transform::new(15.0, 25.0);
        let mut node = Node2D::new("Test");
        node.from_transform(&tf);

        assert_eq!(node.x(), 15.0);
        assert_eq!(node.y(), 25.0);
    }

    #[test]
    fn test_node2d_z_index() {
        let mut node = Node2D::new("Test");
        node.set_z_index(10);
        assert_eq!(node.z_index(), 10);
    }

    #[test]
    fn test_node2d_to_string() {
        let node = Node2D::new("TestNode");
        let str_repr = node.to_string();
        assert!(str_repr.contains("TestNode"));
        assert!(str_repr.contains("pos="));
        assert!(str_repr.contains("rot="));
    }
}
