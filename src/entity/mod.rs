pub mod manager;

pub use manager::EntityManager;

/// Entity ID — Her entity'nin unique bir 32-bit ID'si vardır
/// 
/// # Örnek
/// ```
/// use mantle_engine::entity::EntityId;
/// let id = EntityId::new(42);
/// ```
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct EntityId(pub u32);

impl EntityId {
    pub fn new(id: u32) -> Self {
        EntityId(id)
    }

    pub fn as_u32(&self) -> u32 {
        self.0
    }
}

impl std::fmt::Display for EntityId {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Entity({})", self.0)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_entity_id_creation() {
        let id1 = EntityId::new(1);
        let id2 = EntityId::new(2);
        
        assert_eq!(id1.as_u32(), 1);
        assert_eq!(id2.as_u32(), 2);
        assert_ne!(id1, id2);
    }

    #[test]
    fn test_entity_id_equality() {
        let id1 = EntityId::new(42);
        let id2 = EntityId::new(42);
        let id3 = EntityId::new(43);
        
        assert_eq!(id1, id2);
        assert_ne!(id1, id3);
    }

    #[test]
    fn test_entity_id_hash() {
        use std::collections::HashMap;
        
        let mut map = HashMap::new();
        let id = EntityId::new(100);
        map.insert(id, "test");
        
        assert_eq!(map.get(&EntityId::new(100)), Some(&"test"));
    }

    #[test]
    fn test_entity_id_display() {
        let id = EntityId::new(7);
        assert_eq!(format!("{}", id), "Entity(7)");
    }
}
