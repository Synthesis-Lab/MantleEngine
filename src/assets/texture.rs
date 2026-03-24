use std::collections::HashMap;
use crate::log_info;

/// Texture ID — Her texture'ın unique bir ID'si vardır
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct TextureId(pub u32);

impl TextureId {
    pub fn new(id: u32) -> Self {
        TextureId(id)
    }

    pub fn as_u32(&self) -> u32 {
        self.0
    }
}

impl std::fmt::Display for TextureId {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Texture({})", self.0)
    }
}

/// Texture verileri — Path, boyut bilgimleri
#[derive(Debug, Clone)]
pub struct TextureData {
    /// Dosya yolu (ör: "assets/player.png")
    pub path: String,
    /// Texture genişliği (piksel)
    pub width: u32,
    /// Texture yüksekliği (piksel)
    pub height: u32,
}

impl TextureData {
    pub fn new(path: String, width: u32, height: u32) -> Self {
        Self { path, width, height }
    }

    /// Aspect ratio'yu (width/height) hesapla
    pub fn aspect_ratio(&self) -> f32 {
        self.width as f32 / self.height as f32
    }
}

/// Asset Manager — Texture depolama ve yönetimi
#[derive(Debug)]
pub struct AssetManager {
    /// Texture storage
    textures: HashMap<TextureId, TextureData>,
    /// Sonraki texture ID counter
    next_id: u32,
}

impl AssetManager {
    /// Yeni AssetManager oluştur
    pub fn new() -> Self {
        Self {
            textures: HashMap::new(),
            next_id: 0,
        }
    }

    /// Yeni bir texture yükle
    ///
    /// # Örnek
    /// ```
    /// use mantle_engine::assets::AssetManager;
    /// let mut manager = AssetManager::new();
    /// let id = manager.load_texture("assets/player.png", 64, 64);
    /// ```
    pub fn load_texture(&mut self, path: String, width: u32, height: u32) -> TextureId {
        let id = TextureId::new(self.next_id);
        self.next_id += 1;

        let texture = TextureData::new(path.clone(), width, height);
        self.textures.insert(id, texture);

        log_info!("Texture {} yüklendi: {} ({}x{})", id, path, width, height);
        id
    }

    /// Texture verilerini al
    pub fn get_texture(&self, id: TextureId) -> Option<TextureData> {
        self.textures.get(&id).cloned()
    }

    /// Texture'un var olup olmadığını kontrol et
    pub fn contains_texture(&self, id: TextureId) -> bool {
        self.textures.contains_key(&id)
    }

    /// Kaç texture yüklü
    pub fn texture_count(&self) -> usize {
        self.textures.len()
    }

    /// Tüm texture'ları temizle
    pub fn clear(&mut self) {
        self.textures.clear();
        log_info!("Tüm texture'lar temizlendi");
    }

    /// Tüm yüklü texture ID'lerini al
    pub fn texture_ids(&self) -> Vec<TextureId> {
        self.textures.keys().copied().collect()
    }
}

impl Default for AssetManager {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_texture_id_creation() {
        let id1 = TextureId::new(1);
        let id2 = TextureId::new(2);

        assert_eq!(id1.as_u32(), 1);
        assert_eq!(id2.as_u32(), 2);
        assert_ne!(id1, id2);
    }

    #[test]
    fn test_texture_id_display() {
        let id = TextureId::new(5);
        assert_eq!(format!("{}", id), "Texture(5)");
    }

    #[test]
    fn test_texture_data_creation() {
        let tex = TextureData::new("assets/test.png".to_string(), 256, 256);
        assert_eq!(tex.width, 256);
        assert_eq!(tex.height, 256);
        assert_eq!(tex.path, "assets/test.png");
    }

    #[test]
    fn test_texture_aspect_ratio() {
        let tex1 = TextureData::new("test.png".to_string(), 128, 64);
        assert!((tex1.aspect_ratio() - 2.0).abs() < 0.01);

        let tex2 = TextureData::new("test.png".to_string(), 64, 128);
        assert!((tex2.aspect_ratio() - 0.5).abs() < 0.01);
    }

    #[test]
    fn test_asset_manager_creation() {
        let manager = AssetManager::new();
        assert_eq!(manager.texture_count(), 0);
    }

    #[test]
    fn test_load_texture() {
        let mut manager = AssetManager::new();
        let id = manager.load_texture("assets/player.png".to_string(), 64, 64);

        assert!(manager.contains_texture(id));
        assert_eq!(manager.texture_count(), 1);

        let tex = manager.get_texture(id).unwrap();
        assert_eq!(tex.width, 64);
        assert_eq!(tex.height, 64);
        assert_eq!(tex.path, "assets/player.png");
    }

    #[test]
    fn test_load_multiple_textures() {
        let mut manager = AssetManager::new();
        let id1 = manager.load_texture("player.png".to_string(), 64, 64);
        let id2 = manager.load_texture("enemy.png".to_string(), 32, 32);
        let id3 = manager.load_texture("background.png".to_string(), 1024, 768);

        assert_eq!(manager.texture_count(), 3);
        assert_ne!(id1, id2);
        assert_ne!(id2, id3);

        // ID'lerin ardışık olduğunu kontrol et
        assert_eq!(id1.as_u32(), 0);
        assert_eq!(id2.as_u32(), 1);
        assert_eq!(id3.as_u32(), 2);
    }

    #[test]
    fn test_texture_ids() {
        let mut manager = AssetManager::new();
        let id1 = manager.load_texture("a.png".to_string(), 64, 64);
        let id2 = manager.load_texture("b.png".to_string(), 64, 64);
        let id3 = manager.load_texture("c.png".to_string(), 64, 64);

        let ids = manager.texture_ids();
        assert_eq!(ids.len(), 3);
        assert!(ids.contains(&id1));
        assert!(ids.contains(&id2));
        assert!(ids.contains(&id3));
    }

    #[test]
    fn test_clear() {
        let mut manager = AssetManager::new();
        manager.load_texture("a.png".to_string(), 64, 64);
        manager.load_texture("b.png".to_string(), 64, 64);

        assert_eq!(manager.texture_count(), 2);
        manager.clear();
        assert_eq!(manager.texture_count(), 0);
    }

    #[test]
    fn test_get_nonexistent_texture() {
        let manager = AssetManager::new();
        let id = TextureId::new(999);

        assert!(manager.get_texture(id).is_none());
        assert!(!manager.contains_texture(id));
    }
}
