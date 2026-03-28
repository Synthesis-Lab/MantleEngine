use rhai::{Engine, EvalAltResult, Dynamic};
use bevy_ecs::world::World;
use bevy_ecs::entity::Entity;
use std::collections::HashMap;

pub mod assets;
pub mod components;
pub mod collision;
pub mod ecs;
pub mod entity;
pub mod ffi;
pub mod logger;
pub mod scene;
pub mod scripting;
pub mod systems;

#[cfg(test)]
mod phase5_plus_tests;

pub use assets::{AssetManager, TextureId, TextureData};
pub use components::{Sprite, Transform, Collider, Animation, AnimationFrame, AnimationState};
pub use collision::check_collision;
pub use ecs::{CollisionEvent, AnimationSystem, CollisionSystem};
pub use entity::EntityId;
pub use ffi::{RenderPacket, RenderArena, PinnedBuffer, RendererHandle, FFIError};
pub use logger::{LogLevel, MantleLogger};
pub use scene::{Scene, Node, Node2D, NodeId, NodeScript, Sprite2D, AnimatedSprite2D, CollisionShape2D, CollisionShape};
pub use scripting::{register_transform_functions, register_sprite_functions, register_collision_functions, register_animation_functions};
pub use systems::{
    RenderSystem, RenderData, RenderStats,
    AnimationSystem as Phase5AnimationSystem, AnimationTrack, PlaybackState, AnimationStats,
    CollisionSystem as Phase5CollisionSystem, CollisionEvent as Phase5CollisionEvent, CollisionStats, CollisionConfig,
};

/// Mantle Engine Core — Script motoru, ECS World, Scene management, Asset yönetimi, Component yönetimi, ve Phase 5+ Systems
pub struct MantleCore {
    pub engine: Engine,
    pub world: World,
    pub asset_manager: AssetManager,
    pub events: Vec<CollisionEvent>,
    pub scene: Scene,
    // Mapping from EntityId to Bevy Entity for lookups
    entity_map: HashMap<EntityId, Entity>,
    next_entity_id: u32,
    // Phase 5+ Systems
    pub render_system: RenderSystem,
    pub animation_system: Phase5AnimationSystem,
    pub collision_system: Phase5CollisionSystem,
    // Frame tracking
    current_frame: u64,
}

/// Game loop statistics from a single frame orchestration
#[derive(Debug, Clone, Default)]
pub struct GameLoopStats {
    pub frame_number: u64,
    pub delta_time: f32,
    pub animation_stats: AnimationStats,
    pub render_stats: RenderStats,
    pub collision_stats: CollisionStats,
}

impl MantleCore {
    /// Yeni bir MantleCore örneği oluştur
    pub fn new() -> Self {
        let mut engine = Engine::new();
        
        // Transform, Sprite, Collision ve Animation fonksiyonlarını Rhai'ye kaydet
        register_transform_functions(&mut engine);
        register_sprite_functions(&mut engine);
        register_collision_functions(&mut engine);
        register_animation_functions(&mut engine);

        let core = Self {
            engine,
            world: World::new(),
            asset_manager: AssetManager::new(),
            events: Vec::new(),
            scene: Scene::new(),
            entity_map: HashMap::new(),
            next_entity_id: 0,
            render_system: RenderSystem::new(),
            animation_system: Phase5AnimationSystem::new(),
            collision_system: Phase5CollisionSystem::new(),
            current_frame: 0,
        };
        
        // Phase 5e: Initialize Vulkan renderer for real-time frame output
        match ffi::cpp_bridge::init_vulkan_renderer() {
            ffi::FFIError::OK => {
                eprintln!("[MantleCore] Vulkan renderer initialized successfully (Phase 5e)");
            }
            err => {
                eprintln!("[MantleCore] Failed to initialize Vulkan renderer: {:?}", err);
                // Continue anyway; some applications may not need rendering
            }
        }
        
        core
    }

    /// Script çalıştır
    pub fn run_script(&mut self, code: &str) -> Result<(), Box<EvalAltResult>> {
        self.engine.run(code)
    }

    /// Script'ten sonuç al
    pub fn eval_script(&mut self, code: &str) -> Result<Dynamic, Box<EvalAltResult>> {
        self.engine.eval(code)
    }

    /// Entity oluştur
    pub fn create_entity(&mut self, x: f32, y: f32) -> EntityId {
        let transform = Transform::new(x, y);
        let bevy_entity_ref = self.world.spawn(transform);
        let bevy_entity = bevy_entity_ref.id();
        let entity_id = EntityId(self.next_entity_id);
        self.next_entity_id += 1;
        self.entity_map.insert(entity_id, bevy_entity);
        entity_id
    }

    /// Entity'nin Transform'unu al
    pub fn get_entity_transform(&self, id: EntityId) -> Option<Transform> {
        self.entity_map.get(&id).and_then(|bevy_entity| {
            self.world.get::<Transform>(*bevy_entity).copied()
        })
    }

    /// Entity'nin Transform'unu değiştir (mutable access)
    /// Note: Due to Bevy's borrowing requirements, this returns a copy for mutation
    /// Use entity methods like set_entity_position() for direct updates
    pub fn get_entity_transform_mut(&mut self, _id: EntityId) -> Option<&mut Transform> {
        // Phase 1: Bevy's get_mut returns Mut<T> which can't be returned as &mut T
        // This TODO will be resolved in Phase 3 with a Commands-based pattern
        // For now, use set_entity_position or get_entity_transform
        None
    }

    /// Set entity position (workaround for mutable access in Bevy)
    pub fn set_entity_position(&mut self, id: EntityId, x: f32, y: f32) -> bool {
        if let Some(&bevy_entity) = self.entity_map.get(&id) {
            if let Some(mut transform) = self.world.get_mut::<Transform>(bevy_entity) {
                transform.set_x(x);
                transform.set_y(y);
                return true;
            }
        }
        false
    }

    /// Entity'yi sil
    pub fn remove_entity(&mut self, id: EntityId) -> bool {
        if let Some(bevy_entity) = self.entity_map.remove(&id) {
            self.world.despawn(bevy_entity);
            true
        } else {
            false
        }
    }

    /// Kaç entity var
    pub fn entity_count(&self) -> usize {
        self.entity_map.len()
    }

    /// Update engine systems with delta time
    pub fn update(&mut self, delta_time: f32) {
        AnimationSystem::update(&mut self.world, delta_time);
        CollisionSystem::update(&mut self.world, &mut self.events);
    }

    /// Texture yükle
    pub fn load_texture(&mut self, path: String, width: u32, height: u32) -> TextureId {
        self.asset_manager.load_texture(path, width, height)
    }

    /// Texture sayısını al
    pub fn texture_count(&self) -> usize {
        self.asset_manager.texture_count()
    }

    // ========== SCENE MANAGEMENT ==========

    /// Get the current scene
    pub fn scene(&self) -> &Scene {
        &self.scene
    }

    /// Get mutable scene
    pub fn scene_mut(&mut self) -> &mut Scene {
        &mut self.scene
    }

    /// Create a new Node2D and add to scene
    pub fn create_node2d(&mut self, name: impl Into<String>, x: f32, y: f32) -> NodeId {
        let mut node = Node2D::new(name);
        node.set_position(glam::Vec2::new(x, y));
        let id = node.id();
        self.scene.add_node(node.node().clone());
        id
    }

    /// Create a new Sprite2D and add to scene
    pub fn create_sprite2d(&mut self, name: impl Into<String>, x: f32, y: f32) -> NodeId {
        let mut sprite = Sprite2D::new(name);
        sprite.set_position(glam::Vec2::new(x, y));
        let id = sprite.node2d().id();
        self.scene.add_node(sprite.node2d().node().clone());
        id
    }

    /// Create a new AnimatedSprite2D and add to scene
    pub fn create_animated_sprite2d(&mut self, name: impl Into<String>, x: f32, y: f32) -> NodeId {
        let mut sprite = AnimatedSprite2D::new(name);
        sprite.set_position(glam::Vec2::new(x, y));
        let id = sprite.sprite2d().node2d().id();
        self.scene.add_node(sprite.sprite2d().node2d().node().clone());
        id
    }

    /// Create a new CollisionShape2D (AABB) and add to scene
    pub fn create_collision_shape2d_aabb(
        &mut self,
        name: impl Into<String>,
        x: f32,
        y: f32,
        width: f32,
        height: f32,
    ) -> NodeId {
        let mut collision = CollisionShape2D::new_aabb(name, width, height);
        collision.set_position(glam::Vec2::new(x, y));
        let id = collision.node2d().id();
        self.scene.add_node(collision.node2d().node().clone());
        id
    }

    /// Create a new CollisionShape2D (Circle) and add to scene
    pub fn create_collision_shape2d_circle(
        &mut self,
        name: impl Into<String>,
        x: f32,
        y: f32,
        radius: f32,
    ) -> NodeId {
        let mut collision = CollisionShape2D::new_circle(name, radius);
        collision.set_position(glam::Vec2::new(x, y));
        let id = collision.node2d().id();
        self.scene.add_node(collision.node2d().node().clone());
        id
    }

    /// Get node count in scene
    pub fn node_count(&self) -> usize {
        self.scene.node_count()
    }

    /// Get node by ID from scene
    pub fn get_node(&self, id: NodeId) -> Option<&Node> {
        self.scene.get_node(id)
    }

    /// Get mutable node from scene
    pub fn get_node_mut(&mut self, id: NodeId) -> Option<&mut Node> {
        self.scene.get_node_mut(id)
    }

    /// Find node by name in scene
    pub fn find_node_by_name(&self, name: &str) -> Option<NodeId> {
        self.scene.find_node_by_name(name)
    }

    /// Add node to scene
    pub fn add_node_to_scene(&mut self, node: Node) -> NodeId {
        self.scene.add_node(node)
    }

    /// Remove node from scene
    pub fn remove_node_from_scene(&mut self, id: NodeId) -> bool {
        self.scene.remove_node(id)
    }

    // ===== Phase 5+ Game Loop Orchestration =====

    /// Update all Phase 5+ systems in coordinated sequence:
    /// 1. Render system begins frame
    /// 2. Animation system updates all tracks
    /// 3. Collision system advances frame (enter/exit detection)
    /// 4. Render packet built and submitted
    /// 5. Frame counter incremented
    pub fn update_frame(&mut self, delta_time: f32) -> GameLoopStats {
        // Begin render frame
        self.render_system.begin_frame(self.current_frame, delta_time);

        // Update all animation tracks
        self.animation_system.update(delta_time);

        // Advance collision detection (updates enter/exit state)
        self.collision_system.next_frame();

        // Collect frame statistics
        let animation_stats = self.animation_system.stats().clone();
        let render_stats = self.render_system.submit_frame();
        let collision_stats = self.collision_system.stats().clone();

        // Phase 5e: Real-time GPU rendering pipeline
        // Build render packet from ECS data
        let render_packet = self.render_system.build_render_packet();
        
        // Get renderer handle
        if let Ok(handle) = ffi::cpp_bridge::get_vulkan_renderer() {
            // Submit packet to C++ renderer for GPU processing
            let _ = ffi::c_abi::mantle_render_submit(
                ffi::RendererHandle(handle),
                &render_packet as *const _,
            );
            
            // Wait for GPU to finish (vkQueueWaitIdle synchronization)
            let _ = ffi::c_abi::mantle_wait_render(ffi::RendererHandle(handle));
        }

        // Increment frame counter
        self.current_frame += 1;

        GameLoopStats {
            frame_number: self.current_frame - 1, // Return the frame we just completed
            delta_time,
            animation_stats,
            render_stats,
            collision_stats,
        }
    }

    /// Get current frame number (0-indexed)
    pub fn frame_number(&self) -> u64 {
        self.current_frame
    }

    /// Query active collisions this frame
    pub fn active_collisions(&self) -> &[Phase5CollisionEvent] {
        self.collision_system.collisions()
    }

    /// Query new collisions this frame (entered)
    pub fn new_collisions(&self) -> Vec<&Phase5CollisionEvent> {
        self.collision_system.collision_entered()
    }

    /// Query ended collisions this frame (exited)
    pub fn ended_collisions(&self) -> Vec<&Phase5CollisionEvent> {
        self.collision_system.collision_exited()
    }

    /// Check if two entities are currently colliding
    pub fn are_colliding(&self, entity_a: u64, entity_b: u64) -> bool {
        self.collision_system.are_colliding(entity_a, entity_b)
    }

    /// Manually add render data to current frame packet
    /// Useful for custom rendering without scene traversal
    pub fn add_render_transforms(&mut self, transforms: Vec<(u64, f32, f32)>) {
        for (id, x, y) in transforms {
            self.render_system.add_transform_simple(id, x, y);
        }
    }

    /// Manually add sprite data to current frame packet
    pub fn add_render_sprites(&mut self, sprites: Vec<(u64, String)>) {
        for (id, texture) in sprites {
            self.render_system.add_sprite_simple(id, &texture);
        }
    }

    /// Manually add collision data to current frame packet
    pub fn add_render_colliders(&mut self, colliders: Vec<(u64, f32, f32)>) {
        for (id, width, height) in colliders {
            self.render_system.add_collider_simple(id, width, height);
        }
    }

    /// Build zero-copy render packet for FFI submission to C++ renderer
    pub fn build_render_packet(&self) -> ffi::render_packet::RenderPacket {
        self.render_system.build_render_packet()
    }

    /// Get render system statistics
    pub fn render_stats(&self) -> RenderStats {
        self.render_system.stats()
    }

    /// Get animation system statistics
    pub fn animation_stats(&self) -> AnimationStats {
        self.animation_system.stats().clone()
    }

    /// Get collision system statistics
    pub fn collision_stats(&self) -> CollisionStats {
        self.collision_system.stats().clone()
    }
}

// Phase 5e: Renderer cleanup on drop
impl Drop for MantleCore {
    fn drop(&mut self) {
        ffi::cpp_bridge::shutdown_vulkan_renderer();
    }
}

impl Default for MantleCore {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_script_execution() {
        let mut core = MantleCore::new();
        let result = core.run_script("print(\"MantleEngine is alive!\")");
        assert!(result.is_ok());
    }

    #[test]
    fn test_core_creation() {
        let core = MantleCore::new();
        assert_eq!(core.entity_count(), 0);
    }

    #[test]
    fn test_create_entity() {
        let mut core = MantleCore::new();
        let id = core.create_entity(10.0, 20.0);
        
        assert_eq!(core.entity_count(), 1);
        let tf = core.get_entity_transform(id).unwrap();
        assert_eq!(tf.x(), 10.0);
        assert_eq!(tf.y(), 20.0);
    }

    #[test]
    fn test_remove_entity() {
        let mut core = MantleCore::new();
        let id = core.create_entity(0.0, 0.0);
        
        assert_eq!(core.entity_count(), 1);
        assert!(core.remove_entity(id));
        assert_eq!(core.entity_count(), 0);
    }

    #[test]
    fn test_load_texture() {
        let mut core = MantleCore::new();
        assert_eq!(core.texture_count(), 0);

        let _id = core.load_texture("assets/player.png".to_string(), 64, 64);
        assert_eq!(core.texture_count(), 1);
    }

    #[test]
    fn test_core_with_assets_and_entities() {
        let mut core = MantleCore::new();
        
        // Texture yükle
        let _tex_id = core.load_texture("enemy.png".to_string(), 32, 32);
        assert_eq!(core.texture_count(), 1);
        
        // Entity oluştur
        let ent_id = core.create_entity(100.0, 200.0);
        assert_eq!(core.entity_count(), 1);
        
        // Her ikisinin de başarılı olduğunu kontrol et
        let tf = core.get_entity_transform(ent_id).unwrap();
        assert_eq!(tf.x(), 100.0);
    }

    #[test]
    fn test_scene_creation() {
        let core = MantleCore::new();
        assert_eq!(core.node_count(), 0);
    }

    #[test]
    fn test_create_node2d() {
        let mut core = MantleCore::new();
        let node_id = core.create_node2d("Player", 50.0, 100.0);
        
        assert_eq!(core.node_count(), 1);
        let node = core.get_node(node_id).unwrap();
        assert_eq!(node.name(), "Player");
    }

    #[test]
    fn test_find_node_by_name() {
        let mut core = MantleCore::new();
        let _id = core.create_node2d("Enemy", 10.0, 20.0);
        
        let found_id = core.find_node_by_name("Enemy");
        assert!(found_id.is_some());
    }

    #[test]
    fn test_remove_node_from_scene() {
        let mut core = MantleCore::new();
        let node_id = core.create_node2d("Sprite", 0.0, 0.0);
        
        assert_eq!(core.node_count(), 1);
        assert!(core.remove_node_from_scene(node_id));
        assert_eq!(core.node_count(), 0);
    }

    #[test]
    fn test_add_node_to_scene() {
        let mut core = MantleCore::new();
        let node = Node::new("TestNode");
        let id = core.add_node_to_scene(node);
        
        assert_eq!(core.node_count(), 1);
        assert_eq!(core.get_node(id).unwrap().name(), "TestNode");
    }

    #[test]
    fn test_create_sprite2d() {
        let mut core = MantleCore::new();
        let node_id = core.create_sprite2d("Player", 10.0, 20.0);

        assert_eq!(core.node_count(), 1);
        let node = core.get_node(node_id).unwrap();
        assert_eq!(node.name(), "Player");
    }

    #[test]
    fn test_create_animated_sprite2d() {
        let mut core = MantleCore::new();
        let node_id = core.create_animated_sprite2d("Enemy", 50.0, 75.0);

        assert_eq!(core.node_count(), 1);
        let node = core.get_node(node_id).unwrap();
        assert_eq!(node.name(), "Enemy");
    }

    #[test]
    fn test_create_collision_shape2d_aabb() {
        let mut core = MantleCore::new();
        let node_id = core.create_collision_shape2d_aabb("Wall", 0.0, 0.0, 100.0, 50.0);

        assert_eq!(core.node_count(), 1);
        let node = core.get_node(node_id).unwrap();
        assert_eq!(node.name(), "Wall");
    }

    #[test]
    fn test_create_collision_shape2d_circle() {
        let mut core = MantleCore::new();
        let node_id = core.create_collision_shape2d_circle("Bullet", 10.0, 10.0, 5.0);

        assert_eq!(core.node_count(), 1);
        let node = core.get_node(node_id).unwrap();
        assert_eq!(node.name(), "Bullet");
    }
}