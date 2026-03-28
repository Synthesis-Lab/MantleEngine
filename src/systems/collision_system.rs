//! CollisionSystem - Phase 5 Collision Detection & Response
//!
//! Tracks collision events and manages collision resolution.
//! Works with collision shapes and generates events for scripting.

/// Represents a collision event between two entities
#[derive(Clone, Debug)]
pub struct CollisionEvent {
    pub entity_a: u64,
    pub entity_b: u64,
    pub contact_x: f32,
    pub contact_y: f32,
}

/// Collision resolver configuration
#[derive(Clone, Copy)]
pub struct CollisionConfig {
    /// Enable collision response (physics)
    pub enable_response: bool,
    /// Restitution coefficient (bounce)
    pub restitution: f32,
    /// Friction coefficient
    pub friction: f32,
}

impl Default for CollisionConfig {
    fn default() -> Self {
        Self {
            enable_response: true,
            restitution: 0.8,
            friction: 0.2,
        }
    }
}

/// CollisionSystem - Detects and manages collisions
pub struct CollisionSystem {
    /// Current frame's collision events
    current_events: Vec<CollisionEvent>,
    /// Previous frame's collision events (for on_enter/on_exit)
    previous_events: Vec<CollisionEvent>,
    /// Total collisions detected lifetime
    total_collisions: u64,
    /// Configuration
    config: CollisionConfig,
}

impl CollisionSystem {
    /// Create new collision system
    pub fn new() -> Self {
        Self {
            current_events: Vec::new(),
            previous_events: Vec::new(),
            total_collisions: 0,
            config: CollisionConfig::default(),
        }
    }

    /// Set system configuration
    pub fn set_config(&mut self, config: CollisionConfig) {
        self.config = config;
    }

    /// Report a collision (called by collision detector)
    pub fn report_collision(&mut self, entity_a: u64, entity_b: u64, contact_x: f32, contact_y: f32) {
        let event = CollisionEvent {
            entity_a,
            entity_b,
            contact_x,
            contact_y,
        };
        self.current_events.push(event);
        self.total_collisions += 1;
    }

    /// Get all collisions this frame
    pub fn collisions(&self) -> &[CollisionEvent] {
        &self.current_events
    }

    /// Get collision count this frame
    pub fn collision_count(&self) -> usize {
        self.current_events.len()
    }

    /// Get collisions by entity
    pub fn collisions_for_entity(&self, entity_id: u64) -> Vec<&CollisionEvent> {
        self.current_events
            .iter()
            .filter(|e| e.entity_a == entity_id || e.entity_b == entity_id)
            .collect()
    }

    /// Check if two entities collided this frame
    pub fn are_colliding(&self, entity_a: u64, entity_b: u64) -> bool {
        self.current_events.iter().any(|e| {
            (e.entity_a == entity_a && e.entity_b == entity_b)
                || (e.entity_a == entity_b && e.entity_b == entity_a)
        })
    }

    /// Get collisions that started this frame (not in previous frame)
    pub fn collision_entered(&self) -> Vec<&CollisionEvent> {
        self.current_events
            .iter()
            .filter(|current| {
                !self.previous_events.iter().any(|prev| {
                    (prev.entity_a == current.entity_a && prev.entity_b == current.entity_b)
                        || (prev.entity_a == current.entity_b && prev.entity_b == current.entity_a)
                })
            })
            .collect()
    }

    /// Get collisions that ended this frame (in previous but not current)
    pub fn collision_exited(&self) -> Vec<&CollisionEvent> {
        self.previous_events
            .iter()
            .filter(|prev| {
                !self.current_events.iter().any(|current| {
                    (current.entity_a == prev.entity_a && current.entity_b == prev.entity_b)
                        || (current.entity_a == prev.entity_b && current.entity_b == prev.entity_a)
                })
            })
            .collect()
    }

    /// Advance to next frame (clear current collisions, save for next frame)
    pub fn next_frame(&mut self) {
        self.previous_events = self.current_events.clone();
        self.current_events.clear();
    }

    /// Get statistics
    pub fn stats(&self) -> CollisionStats {
        CollisionStats {
            current_count: self.current_events.len(),
            entered_count: self.collision_entered().len(),
            exited_count: self.collision_exited().len(),
            total_lifetime: self.total_collisions,
        }
    }

    /// Reset system
    pub fn reset(&mut self) {
        self.current_events.clear();
        self.previous_events.clear();
        self.total_collisions = 0;
    }
}

impl Default for CollisionSystem {
    fn default() -> Self {
        Self::new()
    }
}

/// Statistics about collision detection
#[derive(Debug, Clone, Default)]
pub struct CollisionStats {
    pub current_count: usize,
    pub entered_count: usize,
    pub exited_count: usize,
    pub total_lifetime: u64,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_collision_system_creation() {
        let system = CollisionSystem::new();
        assert_eq!(system.collision_count(), 0);
    }

    #[test]
    fn test_report_collision() {
        let mut system = CollisionSystem::new();
        system.report_collision(1, 2, 10.0, 20.0);

        assert_eq!(system.collision_count(), 1);
        assert!(system.are_colliding(1, 2));
        assert!(system.are_colliding(2, 1)); // order doesn't matter
    }

    #[test]
    fn test_collisions_for_entity() {
        let mut system = CollisionSystem::new();
        system.report_collision(1, 2, 0.0, 0.0);
        system.report_collision(1, 3, 0.0, 0.0);
        system.report_collision(2, 3, 0.0, 0.0);

        let entity_1_collisions = system.collisions_for_entity(1);
        assert_eq!(entity_1_collisions.len(), 2);
    }

    #[test]
    fn test_collision_entered() {
        let mut system = CollisionSystem::new();

        // First frame: collision happens
        system.report_collision(1, 2, 0.0, 0.0);
        let entered = system.collision_entered();
        assert_eq!(entered.len(), 1);

        // Move to next frame without reporting
        system.next_frame();
        let entered = system.collision_entered();
        assert_eq!(entered.len(), 0);

        // New collision
        system.report_collision(1, 3, 0.0, 0.0);
        let entered = system.collision_entered();
        assert_eq!(entered.len(), 1);
    }

    #[test]
    fn test_collision_exited() {
        let mut system = CollisionSystem::new();

        // First frame: collision
        system.report_collision(1, 2, 0.0, 0.0);
        system.next_frame();

        // Second frame: no collision - should detect exit
        let exited = system.collision_exited();
        assert_eq!(exited.len(), 1);
    }

    #[test]
    fn test_collision_persistence() {
        let mut system = CollisionSystem::new();

        // Frame 1: collision
        system.report_collision(1, 2, 0.0, 0.0);
        system.next_frame();

        // Frame 2: same collision (not new)
        system.report_collision(1, 2, 0.0, 0.0);
        assert_eq!(system.collision_entered().len(), 0); // not "entered"
        assert_eq!(system.collision_exited().len(), 0); // not "exited"
    }

    #[test]
    fn test_collision_stats() {
        let mut system = CollisionSystem::new();
        system.report_collision(1, 2, 0.0, 0.0);
        system.report_collision(1, 3, 0.0, 0.0);

        let stats = system.stats();
        assert_eq!(stats.current_count, 2);
        assert_eq!(stats.entered_count, 2);
        assert_eq!(stats.total_lifetime, 2);
    }

    #[test]
    fn test_reset() {
        let mut system = CollisionSystem::new();
        system.report_collision(1, 2, 0.0, 0.0);
        system.reset();

        assert_eq!(system.collision_count(), 0);
        assert_eq!(system.total_collisions, 0);
    }
}

