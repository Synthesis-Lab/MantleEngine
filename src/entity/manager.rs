use std::collections::HashMap;
use crate::components::Transform;
use crate::entity::EntityId;
use crate::log_info;

/// Entity Manager — Entity oluşturma, silme ve Transform storage
#[derive(Debug)]
pub struct EntityManager {
    /// Entity Transform storage
    transforms: HashMap<EntityId, Transform>,
    /// Sonraki entity ID counter
    next_id: u32,
}

impl EntityManager {
    /// Yeni EntityManager oluştur
    pub fn new() -> Self {
        Self {
            transforms: HashMap::new(),
            next_id: 0,
        }
    }

    /// Yeni bir entity oluştur ve Transform'u konum ile başlat
    /// 
    /// # Örnek
    /// ```
    /// use mantle_engine::entity::EntityManager;
    /// let mut manager = EntityManager::new();
    /// let id = manager.create_entity(10.0, 20.0);
    /// ```
    pub fn create_entity(&mut self, x: f32, y: f32) -> EntityId {
        let id = EntityId::new(self.next_id);
        self.next_id += 1;

        let transform = Transform::new(x, y);
        self.transforms.insert(id, transform);

        log_info!("Entity {} oluşturuldu (x={}, y={})", id, x, y);
        id
    }

    /// Bir entity'nin Transform'unu al
    pub fn get_transform(&self, id: EntityId) -> Option<Transform> {
        self.transforms.get(&id).copied()
    }

    /// Bir entity'nin Transform'unu değiştir (mutable referens)
    pub fn get_transform_mut(&mut self, id: EntityId) -> Option<&mut Transform> {
        self.transforms.get_mut(&id)
    }

    /// Entity'yi sil
    pub fn remove_entity(&mut self, id: EntityId) -> bool {
        let removed = self.transforms.remove(&id).is_some();
        if removed {
            log_info!("Entity {} silindi", id);
        }
        removed
    }

    /// Tüm entity'leri sil
    pub fn clear(&mut self) {
        self.transforms.clear();
        log_info!("Tüm entityler silindi");
    }

    /// Kaç entity var
    pub fn entity_count(&self) -> usize {
        self.transforms.len()
    }

    /// Entity'nin var olup olmadığını kontrol et
    pub fn contains(&self, id: EntityId) -> bool {
        self.transforms.contains_key(&id)
    }

    /// Tüm entity ID'lerini al
    pub fn entity_ids(&self) -> Vec<EntityId> {
        self.transforms.keys().copied().collect()
    }
}

impl Default for EntityManager {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_entity_manager_creation() {
        let manager = EntityManager::new();
        assert_eq!(manager.entity_count(), 0);
    }

    #[test]
    fn test_create_entity() {
        let mut manager = EntityManager::new();
        let id = manager.create_entity(10.0, 20.0);

        assert!(manager.contains(id));
        assert_eq!(manager.entity_count(), 1);

        let tf = manager.get_transform(id).unwrap();
        assert_eq!(tf.x(), 10.0);
        assert_eq!(tf.y(), 20.0);
    }

    #[test]
    fn test_create_multiple_entities() {
        let mut manager = EntityManager::new();
        let id1 = manager.create_entity(0.0, 0.0);
        let id2 = manager.create_entity(100.0, 200.0);
        let id3 = manager.create_entity(-50.0, 75.0);

        assert_eq!(manager.entity_count(), 3);
        assert_ne!(id1, id2);
        assert_ne!(id2, id3);

        // ID'lerin ardışık olduğunu kontrol et
        assert_eq!(id1.as_u32(), 0);
        assert_eq!(id2.as_u32(), 1);
        assert_eq!(id3.as_u32(), 2);
    }

    #[test]
    fn test_get_transform_mut() {
        let mut manager = EntityManager::new();
        let id = manager.create_entity(5.0, 10.0);

        if let Some(tf) = manager.get_transform_mut(id) {
            tf.translate(15.0, 20.0);
        }

        let tf = manager.get_transform(id).unwrap();
        assert_eq!(tf.x(), 20.0);
        assert_eq!(tf.y(), 30.0);
    }

    #[test]
    fn test_remove_entity() {
        let mut manager = EntityManager::new();
        let id = manager.create_entity(0.0, 0.0);

        assert!(manager.contains(id));
        assert!(manager.remove_entity(id));
        assert!(!manager.contains(id));
        assert_eq!(manager.entity_count(), 0);

        // İkinci kez silmeye çalış — false döndür
        assert!(!manager.remove_entity(id));
    }

    #[test]
    fn test_clear() {
        let mut manager = EntityManager::new();
        manager.create_entity(0.0, 0.0);
        manager.create_entity(10.0, 20.0);
        manager.create_entity(30.0, 40.0);

        assert_eq!(manager.entity_count(), 3);
        manager.clear();
        assert_eq!(manager.entity_count(), 0);
    }

    #[test]
    fn test_entity_ids() {
        let mut manager = EntityManager::new();
        let id1 = manager.create_entity(0.0, 0.0);
        let id2 = manager.create_entity(10.0, 20.0);
        let id3 = manager.create_entity(30.0, 40.0);

        let ids = manager.entity_ids();
        assert_eq!(ids.len(), 3);
        assert!(ids.contains(&id1));
        assert!(ids.contains(&id2));
        assert!(ids.contains(&id3));
    }

    #[test]
    fn test_get_nonexistent_entity() {
        let manager = EntityManager::new();
        let id = EntityId::new(999);

        assert!(manager.get_transform(id).is_none());
    }
}
