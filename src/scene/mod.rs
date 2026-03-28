/// Scene management module — Node-based scene tree (Godot-style)
pub mod node;
pub mod node2d;
pub mod sprite2d;
pub mod animated_sprite2d;
pub mod collision_shape2d;

pub use node::{Node, NodeId, NodeScript};
pub use node2d::Node2D;
pub use sprite2d::Sprite2D;
pub use animated_sprite2d::AnimatedSprite2D;
pub use collision_shape2d::{CollisionShape2D, CollisionShape};

use std::collections::HashMap;

/// Scene represents a tree of Nodes
pub struct Scene {
    /// All nodes indexed by ID
    nodes: HashMap<NodeId, Node>,
    /// Root node ID
    root: Option<NodeId>,
}

impl Scene {
    /// Create a new empty scene
    pub fn new() -> Self {
        Scene {
            nodes: HashMap::new(),
            root: None,
        }
    }

    /// Create scene from root node
    pub fn with_root(root: Node) -> Self {
        let mut scene = Scene {
            nodes: HashMap::new(),
            root: Some(root.id()),
        };
        scene.nodes.insert(root.id(), root);
        scene
    }

    /// Get root node
    pub fn root(&self) -> Option<&Node> {
        self.root.and_then(|id| self.nodes.get(&id))
    }

    /// Get mutable root node
    pub fn root_mut(&mut self) -> Option<&mut Node> {
        self.root.and_then(|id| self.nodes.get_mut(&id))
    }

    /// Get node by ID
    pub fn get_node(&self, id: NodeId) -> Option<&Node> {
        self.nodes.get(&id)
    }

    /// Get mutable node by ID
    pub fn get_node_mut(&mut self, id: NodeId) -> Option<&mut Node> {
        self.nodes.get_mut(&id)
    }

    /// Find node by name (recursive from root)
    pub fn find_node_by_name(&self, name: &str) -> Option<NodeId> {
        self.root.and_then(|root_id| self.find_node_by_name_recursive(root_id, name))
    }

    /// Recursive helper for finding nodes by name
    fn find_node_by_name_recursive(&self, node_id: NodeId, name: &str) -> Option<NodeId> {
        if let Some(node) = self.nodes.get(&node_id) {
            if node.name() == name {
                return Some(node_id);
            }
            for child_id in node.children() {
                if let Some(found) = self.find_node_by_name_recursive(*child_id, name) {
                    return Some(found);
                }
            }
        }
        None
    }

    /// Add node to scene (at root level or as child of parent)
    pub fn add_node(&mut self, node: Node) -> NodeId {
        let id = node.id();
        self.nodes.insert(id, node);
        if self.root.is_none() {
            self.root = Some(id);
        }
        id
    }

    /// Add node as child of parent
    pub fn add_child(&mut self, parent_id: NodeId, mut node: Node) -> Option<NodeId> {
        if !self.nodes.contains_key(&parent_id) {
            return None;
        }
        let node_id = node.id();
        node.set_parent(Some(parent_id));
        self.nodes.insert(node_id, node);
        if let Some(parent) = self.nodes.get_mut(&parent_id) {
            parent.add_child_id(node_id);
        }
        Some(node_id)
    }

    /// Remove node (and all children) from scene
    pub fn remove_node(&mut self, id: NodeId) -> bool {
        if let Some(node) = self.nodes.get(&id) {
            let children = node.children().to_vec();
            for child_id in children {
                self.remove_node(child_id);
            }
        }
        self.nodes.remove(&id).is_some()
    }

    /// Get all nodes in scene
    pub fn nodes(&self) -> &HashMap<NodeId, Node> {
        &self.nodes
    }

    /// Get mutable all nodes
    pub fn nodes_mut(&mut self) -> &mut HashMap<NodeId, Node> {
        &mut self.nodes
    }

    /// Total node count
    pub fn node_count(&self) -> usize {
        self.nodes.len()
    }
}

impl Default for Scene {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_scene_creation() {
        let scene = Scene::new();
        assert_eq!(scene.node_count(), 0);
        assert!(scene.root().is_none());
    }

    #[test]
    fn test_scene_with_root() {
        let root = Node::new("Root");
        let scene = Scene::with_root(root);
        assert_eq!(scene.node_count(), 1);
        assert!(scene.root().is_some());
        assert_eq!(scene.root().unwrap().name(), "Root");
    }

    #[test]
    fn test_add_node() {
        let mut scene = Scene::new();
        let node = Node::new("TestNode");
        let id = scene.add_node(node);
        assert_eq!(scene.node_count(), 1);
        assert_eq!(scene.get_node(id).unwrap().name(), "TestNode");
    }

    #[test]
    fn test_parent_child_hierarchy() {
        let mut scene = Scene::new();
        let root = Node::new("Root");
        let root_id = scene.add_node(root);

        let child = Node::new("Child");
        let child_id = scene.add_child(root_id, child).unwrap();

        assert_eq!(scene.node_count(), 2);
        assert_eq!(scene.get_node(child_id).unwrap().parent(), Some(root_id));
        assert!(scene.get_node(root_id).unwrap().children().contains(&child_id));
    }

    #[test]
    fn test_find_node_by_name() {
        let mut scene = Scene::new();
        let root = Node::new("Root");
        let root_id = scene.add_node(root);

        let child = Node::new("MyChild");
        let child_id = scene.add_child(root_id, child).unwrap();

        let found_id = scene.find_node_by_name("MyChild").unwrap();
        assert_eq!(found_id, child_id);
    }

    #[test]
    fn test_remove_node() {
        let mut scene = Scene::new();
        let root = Node::new("Root");
        let root_id = scene.add_node(root);

        let child = Node::new("Child");
        let _child_id = scene.add_child(root_id, child).unwrap();

        assert_eq!(scene.node_count(), 2);
        scene.remove_node(root_id);
        assert_eq!(scene.node_count(), 0);
    }
}
