use std::fmt;
use std::sync::atomic::{AtomicU32, Ordering};

/// Unique identifier for nodes in a scene
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub struct NodeId(u32);

impl NodeId {
    /// Create a new NodeId
    pub fn new(id: u32) -> Self {
        NodeId(id)
    }

    /// Generate a unique NodeId (using atomic counter)
    pub fn generate() -> Self {
        static COUNTER: AtomicU32 = AtomicU32::new(0);
        let id = COUNTER.fetch_add(1, Ordering::Relaxed);
        NodeId(id)
    }

    /// Get the inner u32 value
    pub fn value(&self) -> u32 {
        self.0
    }
}

impl fmt::Display for NodeId {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "NodeId({})", self.0)
    }
}

/// Trait for node lifecycle hooks
/// Scripts implementing this can define behavior at key lifecycle points
pub trait NodeScript {
    /// Called when node enters the scene
    fn ready(&mut self) {}

    /// Called every frame
    fn process(&mut self, _delta_time: f32) {}

    /// Called every physics frame
    fn physics_process(&mut self, _delta_time: f32) {}

    /// Called when node exits the scene
    fn exit_tree(&mut self) {}
}

/// Base Node class — foundation for all scene nodes
/// (Godot-style hierarchy and lifecycle)
#[derive(Clone, Debug)]
pub struct Node {
    /// Unique identifier
    id: NodeId,
    /// Display name in scene tree
    name: String,
    /// Parent node ID (if any)
    parent: Option<NodeId>,
    /// Child node IDs
    children: Vec<NodeId>,
    /// Is this node active? (affects processing)
    active: bool,
    /// Is this node visible? (affects rendering)
    visible: bool,
}

impl Node {
    /// Create a new Node with a name
    pub fn new(name: impl Into<String>) -> Self {
        Node {
            id: NodeId::generate(),
            name: name.into(),
            parent: None,
            children: Vec::new(),
            active: true,
            visible: true,
        }
    }

    /// Create a node with a specific ID (useful for derived types)
    pub fn with_id(name: impl Into<String>, id: NodeId) -> Self {
        Node {
            id,
            name: name.into(),
            parent: None,
            children: Vec::new(),
            active: true,
            visible: true,
        }
    }

    /// Get the node's ID
    pub fn id(&self) -> NodeId {
        self.id
    }

    /// Get the node's name
    pub fn name(&self) -> &str {
        &self.name
    }

    /// Set the node's name
    pub fn set_name(&mut self, name: impl Into<String>) {
        self.name = name.into();
    }

    /// Get the parent node ID
    pub fn parent(&self) -> Option<NodeId> {
        self.parent
    }

    /// Set the parent node ID (internal use)
    pub fn set_parent(&mut self, parent: Option<NodeId>) {
        self.parent = parent;
    }

    /// Get children node IDs
    pub fn children(&self) -> &[NodeId] {
        &self.children
    }

    /// Add a child node ID (internal use)
    pub fn add_child_id(&mut self, child_id: NodeId) {
        if !self.children.contains(&child_id) {
            self.children.push(child_id);
        }
    }

    /// Remove a child node ID (internal use)
    pub fn remove_child_id(&mut self, child_id: NodeId) -> bool {
        if let Some(pos) = self.children.iter().position(|&id| id == child_id) {
            self.children.remove(pos);
            true
        } else {
            false
        }
    }

    /// Get number of children
    pub fn child_count(&self) -> usize {
        self.children.len()
    }

    /// Check if node is active
    pub fn is_active(&self) -> bool {
        self.active
    }

    /// Set node active state
    pub fn set_active(&mut self, active: bool) {
        self.active = active;
    }

    /// Check if node is visible
    pub fn is_visible(&self) -> bool {
        self.visible
    }

    /// Set node visibility
    pub fn set_visible(&mut self, visible: bool) {
        self.visible = visible;
    }

    /// Get string representation (for debugging)
    pub fn to_string(&self) -> String {
        let parent_str = self
            .parent
            .map(|p| format!("parent={}", p.value()))
            .unwrap_or_else(|| "no parent".to_string());
        format!(
            "Node(id={}, name='{}', {}, children={}, active={}, visible={})",
            self.id.value(),
            self.name,
            parent_str,
            self.children.len(),
            self.active,
            self.visible
        )
    }
}

impl Default for Node {
    fn default() -> Self {
        Self::new("Node")
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_node_creation() {
        let node = Node::new("TestNode");
        assert_eq!(node.name(), "TestNode");
        assert!(node.is_active());
        assert!(node.is_visible());
        assert!(node.parent().is_none());
        assert_eq!(node.child_count(), 0);
    }

    #[test]
    fn test_node_id_generation() {
        let id1 = NodeId::generate();
        let id2 = NodeId::generate();
        assert_ne!(id1, id2);
    }

    #[test]
    fn test_node_id_creation() {
        let id = NodeId::new(42);
        assert_eq!(id.value(), 42);
    }

    #[test]
    fn test_node_set_name() {
        let mut node = Node::new("Old");
        node.set_name("New");
        assert_eq!(node.name(), "New");
    }

    #[test]
    fn test_node_parent_child() {
        let parent_id = NodeId::new(1);
        let child_id = NodeId::new(2);

        let mut parent = Node::with_id("Parent", parent_id);
        let mut child = Node::with_id("Child", child_id);

        child.set_parent(Some(parent_id));
        parent.add_child_id(child_id);

        assert_eq!(child.parent(), Some(parent_id));
        assert_eq!(parent.child_count(), 1);
        assert!(parent.children().contains(&child_id));
    }

    #[test]
    fn test_node_remove_child() {
        let parent_id = NodeId::new(1);
        let child_id = NodeId::new(2);

        let mut parent = Node::with_id("Parent", parent_id);
        parent.add_child_id(child_id);
        assert_eq!(parent.child_count(), 1);

        parent.remove_child_id(child_id);
        assert_eq!(parent.child_count(), 0);
    }

    #[test]
    fn test_node_active_state() {
        let mut node = Node::new("Test");
        assert!(node.is_active());

        node.set_active(false);
        assert!(!node.is_active());

        node.set_active(true);
        assert!(node.is_active());
    }

    #[test]
    fn test_node_visibility() {
        let mut node = Node::new("Test");
        assert!(node.is_visible());

        node.set_visible(false);
        assert!(!node.is_visible());

        node.set_visible(true);
        assert!(node.is_visible());
    }

    #[test]
    fn test_node_to_string() {
        let node = Node::new("TestNode");
        let s = node.to_string();
        assert!(s.contains("TestNode"));
        assert!(s.contains("no parent"));
        assert!(s.contains("active=true"));
    }

    #[test]
    fn test_node_id_display() {
        let id = NodeId::new(123);
        assert_eq!(id.to_string(), "NodeId(123)");
    }

    #[test]
    fn test_node_script_trait() {
        struct TestScript;
        impl NodeScript for TestScript {
            fn ready(&mut self) {
                // Test implementation
            }
            fn process(&mut self, _delta: f32) {
                // Test implementation
            }
        }

        let mut script = TestScript;
        script.ready();
        script.process(0.016);
    }
}
