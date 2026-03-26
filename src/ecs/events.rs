use crate::entity::EntityId;
use bevy_ecs::event::Event;

/// Collision detect event — two entities collided
#[derive(Clone, Debug, Event)]
pub struct CollisionEvent {
    /// First entity that collided
    pub entity_a: EntityId,
    /// Second entity that collided
    pub entity_b: EntityId,
}

impl CollisionEvent {
    /// Create a new collision event
    pub fn new(entity_a: EntityId, entity_b: EntityId) -> Self {
        CollisionEvent { entity_a, entity_b }
    }
}
