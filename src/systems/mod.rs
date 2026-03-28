//! Phase 5 Systems - Game logic and rendering integration
//!
//! Systems provide structured implementations of game loop features:
//! - **RenderSystem**: Stages render data and builds zero-copy FFI packets
//! - **AnimationSystem**: Manages animation playback and frame progression  
//! - **CollisionSystem**: Tracks collision events and state
//!
//! Each system is independent and can be integrated into Core as needed.
//!
//! ## Architecture
//!
//! ```text
//! Game Loop
//!   ├── RenderSystem (collect transforms, sprites, colliders)
//!   ├── AnimationSystem (update animation tracks)
//!   └── CollisionSystem (track collision events)
//! ```

pub mod animation_system;
pub mod collision_system;
pub mod render_system;

pub use animation_system::{AnimationStats, AnimationSystem, AnimationTrack, PlaybackState};
pub use collision_system::{CollisionConfig, CollisionEvent, CollisionStats, CollisionSystem};
pub use render_system::{RenderData, RenderStats, RenderSystem};
