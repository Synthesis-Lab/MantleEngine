use rhai::{Engine, EvalAltResult, Dynamic};
use bevy_ecs::world::World;
use bevy_ecs::entity::Entity;
use std::collections::HashMap;

pub mod assets;
pub mod components;
pub mod collision;
pub mod ecs;
pub mod entity;
pub mod logger;
pub mod scripting;

pub use assets::{AssetManager, TextureId, TextureData};
pub use components::{Sprite, Transform, Collider, Animation, AnimationFrame, AnimationState};
pub use collision::check_collision;
pub use ecs::{CollisionEvent, AnimationSystem, CollisionSystem};
pub use entity::EntityId;
pub use logger::{LogLevel, MantleLogger};
pub use scripting::{register_transform_functions, register_sprite_functions, register_collision_functions, register_animation_functions};

/// Mantle Engine Core — Script motoru, ECS World, Asset yönetimi ve Component yönetimi
pub struct MantleCore {
    pub engine: Engine,
    pub world: World,
    pub asset_manager: AssetManager,
    pub events: Vec<CollisionEvent>,
    // Mapping from EntityId to Bevy Entity for lookups
    entity_map: HashMap<EntityId, Entity>,
    next_entity_id: u32,
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

        Self {
            engine,
            world: World::new(),
            asset_manager: AssetManager::new(),
            events: Vec::new(),
            entity_map: HashMap::new(),
            next_entity_id: 0,
        }
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
}