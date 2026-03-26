/// ECS systems module for Bevy ECS integration
pub mod systems;
pub mod events;

pub use systems::{AnimationSystem, CollisionSystem};
pub use events::CollisionEvent;
