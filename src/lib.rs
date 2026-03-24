use rhai::{Engine, EvalAltResult, Dynamic};

pub mod assets;
pub mod components;
pub mod collision;
pub mod entity;
pub mod logger;
pub mod scripting;

pub use assets::{AssetManager, TextureId, TextureData};
pub use components::{Sprite, Transform, Collider};
pub use collision::check_collision;
pub use entity::{EntityId, manager::EntityManager};
pub use logger::{LogLevel, MantleLogger};
pub use scripting::{register_transform_functions, register_sprite_functions, register_collision_functions};

/// Mantle Engine Core — Script motoru, Entity sistemi, Asset yönetimi ve Component yönetimi
pub struct MantleCore {
    pub engine: Engine,
    pub entity_manager: EntityManager,
    pub asset_manager: AssetManager,
}

impl MantleCore {
    /// Yeni bir MantleCore örneği oluştur
    pub fn new() -> Self {
        let mut engine = Engine::new();
        
        // Transform, Sprite ve Collision fonksiyonlarını Rhai'ye kaydet
        register_transform_functions(&mut engine);
        register_sprite_functions(&mut engine);
        register_collision_functions(&mut engine);

        Self {
            engine,
            entity_manager: EntityManager::new(),
            asset_manager: AssetManager::new(),
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
        self.entity_manager.create_entity(x, y)
    }

    /// Entity'nin Transform'unu al
    pub fn get_entity_transform(&self, id: EntityId) -> Option<Transform> {
        self.entity_manager.get_transform(id)
    }

    /// Entity'nin Transform'unu değiştir
    pub fn get_entity_transform_mut(&mut self, id: EntityId) -> Option<&mut Transform> {
        self.entity_manager.get_transform_mut(id)
    }

    /// Entity'yi sil
    pub fn remove_entity(&mut self, id: EntityId) -> bool {
        self.entity_manager.remove_entity(id)
    }

    /// Kaç entity var
    pub fn entity_count(&self) -> usize {
        self.entity_manager.entity_count()
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