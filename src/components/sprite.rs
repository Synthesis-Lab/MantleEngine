use glam::Vec2;
use crate::assets::TextureId;

/// Sprite Component — 2D görsel temsili
///
/// # Notlar
/// - Transform ile beraber kullanılır (konum, rotasyon, ölçek)
/// - Color format: RGBA 0xRRGGBBAA (8-bit per channel)
/// - UV coordinates: (0.0..1.0) aralığında, texture cropping için
///
/// # Örnek
/// ```
/// use mantle_engine::{Sprite, assets::TextureId};
/// let sp = Sprite::new(TextureId::new(0), 64.0, 64.0);
/// // TextureId 0 ile 64x64 sprite oluştur
/// ```
#[derive(Debug, Clone, Copy)]
pub struct Sprite {
    /// Texture ID
    texture_id: TextureId,
    /// Sprite genişliği (piksel)
    width: f32,
    /// Sprite yüksekliği (piksel)
    height: f32,
    /// Renk (RGBA 0xRRGGBBAA)
    color: u32,
    /// UV minimum (crop için)
    uv_min: Vec2,
    /// UV maksimum (crop için)
    uv_max: Vec2,
}

impl Sprite {
    /// Yeni sprite oluştur
    pub fn new(texture_id: TextureId, width: f32, height: f32) -> Self {
        Self {
            texture_id,
            width,
            height,
            color: 0xFFFFFFFF, // beyaz, tam opaque
            uv_min: Vec2::ZERO,
            uv_max: Vec2::ONE,
        }
    }

    // ========== TEXTURE ==========

    /// Texture ID'sini al
    pub fn texture_id(&self) -> TextureId {
        self.texture_id
    }

    /// Texture ID'sini ayarla
    pub fn set_texture_id(&mut self, id: TextureId) {
        self.texture_id = id;
    }

    /// Sprite'ın boyutunu al (width, height)
    pub fn size(&self) -> (f32, f32) {
        (self.width, self.height)
    }

    /// Sprite genişliğini al
    pub fn width(&self) -> f32 {
        self.width
    }

    /// Sprite yüksekliğini al
    pub fn height(&self) -> f32 {
        self.height
    }

    /// Sprite boyutunu ayarla
    pub fn set_size(&mut self, width: f32, height: f32) {
        self.width = width;
        self.height = height;
    }

    // ========== RENK ==========

    /// Reng al (RGBA u32)
    pub fn color(&self) -> u32 {
        self.color
    }

    /// Rengi ayarla (RGBA u32). Format: 0xRRGGBBAA
    ///
    /// # Örnek
    /// ```
    /// use mantle_engine::Sprite;
    /// use mantle_engine::assets::TextureId;
    /// let mut sp = Sprite::new(TextureId::new(0), 64.0, 64.0);
    /// sp.set_color(0xFF0000FF); // kırmızı
    /// sp.set_color(0x00FF00FF); // yeşil
    /// sp.set_color(0x0000FFFF); // mavi
    /// ```
    pub fn set_color(&mut self, color: u32) {
        self.color = color;
    }

    /// Rengin alpha (opaklık) değerini al (0..255)
    pub fn alpha(&self) -> u8 {
        (self.color & 0xFF) as u8
    }

    /// Alpha (opaklık) ayarla (0..255)
    pub fn set_alpha(&mut self, alpha: u8) {
        self.color = (self.color & 0xFFFFFF00) | (alpha as u32);
    }

    /// Rengi hex string olarak al (ör: "#FF0000FF")
    pub fn color_hex(&self) -> String {
        format!("#{:08X}", self.color)
    }

    /// Rengi hex string'den ayarla (ör: "#FF0000FF" veya "FF0000FF")
    pub fn set_color_hex(&mut self, hex: &str) -> Result<(), String> {
        let hex = hex.trim_start_matches('#');
        if hex.len() != 8 {
            return Err("Color hex must be 8 characters (RRGGBBAA)".to_string());
        }

        match u32::from_str_radix(hex, 16) {
            Ok(color) => {
                self.color = color;
                Ok(())
            }
            Err(_) => Err("Invalid hex color".to_string()),
        }
    }

    // ========== UV COORDINATES (Texture Cropping) ==========

    /// UV minimum (crop başlangıcı)
    pub fn uv_min(&self) -> Vec2 {
        self.uv_min
    }

    /// UV maksimum (crop sonu)
    pub fn uv_max(&self) -> Vec2 {
        self.uv_max
    }

    /// UV koordinatlarını ayarla (cropping)
    pub fn set_uv(&mut self, min: Vec2, max: Vec2) {
        self.uv_min = min.max(Vec2::ZERO).min(Vec2::ONE);
        self.uv_max = max.max(Vec2::ZERO).min(Vec2::ONE);
    }

    /// UV'yi rect şeklinde ayarla (x, y, width, height normalized)
    pub fn set_uv_rect(&mut self, x: f32, y: f32, width: f32, height: f32) {
        self.uv_min = Vec2::new(x, y);
        self.uv_max = Vec2::new((x + width).min(1.0), (y + height).min(1.0));
    }

    // ========== BOUNDING BOX ==========

    /// Transform'ün scale'i ile birlikte bounding box boyutunu hesapla
    /// Transform'dan scale alınması gerekir: size * transform.scale()
    pub fn bounding_box_size(&self, scale_x: f32, scale_y: f32) -> (f32, f32) {
        (self.width * scale_x, self.height * scale_y)
    }

    /// Aspect ratio'yu (width/height) hesapla
    pub fn aspect_ratio(&self) -> f32 {
        self.width / self.height
    }

    // ========== RESET ==========

    /// Sprite'ı varsayılan değerlere sıfırla
    pub fn reset(&mut self) {
        self.color = 0xFFFFFFFF;
        self.uv_min = Vec2::ZERO;
        self.uv_max = Vec2::ONE;
    }

    // ========== STRING DÖNÜŞÜMÜ ==========

    pub fn to_string(&self) -> String {
        format!(
            "Sprite {{ texture: {}, size: ({}, {}), color: {}, uv: ({:.2}, {:.2}) -> ({:.2}, {:.2}) }}",
            self.texture_id,
            self.width,
            self.height,
            self.color_hex(),
            self.uv_min.x,
            self.uv_min.y,
            self.uv_max.x,
            self.uv_max.y
        )
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_sprite_creation() {
        let sp = Sprite::new(TextureId::new(0), 64.0, 64.0);
        assert_eq!(sp.texture_id(), TextureId::new(0));
        assert_eq!(sp.width(), 64.0);
        assert_eq!(sp.height(), 64.0);
        assert_eq!(sp.color(), 0xFFFFFFFF); // beyaz
    }

    #[test]
    fn test_sprite_size() {
        let mut sp = Sprite::new(TextureId::new(0), 64.0, 64.0);
        assert_eq!(sp.size(), (64.0, 64.0));

        sp.set_size(128.0, 256.0);
        assert_eq!(sp.width(), 128.0);
        assert_eq!(sp.height(), 256.0);
    }

    #[test]
    fn test_sprite_color() {
        let mut sp = Sprite::new(TextureId::new(0), 64.0, 64.0);
        assert_eq!(sp.color(), 0xFFFFFFFF);

        sp.set_color(0xFF0000FF); // kırmızı
        assert_eq!(sp.color(), 0xFF0000FF);
    }

    #[test]
    fn test_sprite_alpha() {
        let mut sp = Sprite::new(TextureId::new(0), 64.0, 64.0);
        assert_eq!(sp.alpha(), 0xFF);

        sp.set_alpha(0x80); // half opaque
        assert_eq!(sp.alpha(), 0x80);
        assert_eq!(sp.color(), 0xFFFFFF80);
    }

    #[test]
    fn test_sprite_color_hex() {
        let mut sp = Sprite::new(TextureId::new(0), 64.0, 64.0);
        assert_eq!(sp.color_hex(), "#FFFFFFFF");

        sp.set_color(0xFF0000FF);
        assert_eq!(sp.color_hex(), "#FF0000FF");
    }

    #[test]
    fn test_sprite_set_color_hex() {
        let mut sp = Sprite::new(TextureId::new(0), 64.0, 64.0);

        assert!(sp.set_color_hex("FF0000FF").is_ok());
        assert_eq!(sp.color(), 0xFF0000FF);

        assert!(sp.set_color_hex("#00FF00FF").is_ok());
        assert_eq!(sp.color(), 0x00FF00FF);

        // Geçersiz hex
        assert!(sp.set_color_hex("INVALID").is_err());
        assert!(sp.set_color_hex("FF00").is_err());
    }

    #[test]
    fn test_sprite_uv() {
        let mut sp = Sprite::new(TextureId::new(0), 64.0, 64.0);
        assert_eq!(sp.uv_min(), Vec2::ZERO);
        assert_eq!(sp.uv_max(), Vec2::ONE);

        // Half of texture
        sp.set_uv(Vec2::new(0.0, 0.0), Vec2::new(0.5, 0.5));
        assert_eq!(sp.uv_min(), Vec2::new(0.0, 0.0));
        assert_eq!(sp.uv_max(), Vec2::new(0.5, 0.5));
    }

    #[test]
    fn test_sprite_uv_rect() {
        let mut sp = Sprite::new(TextureId::new(0), 64.0, 64.0);

        // Texture atlas: 4x4 grid, 4. tile'ı al
        sp.set_uv_rect(0.75, 0.75, 0.25, 0.25);
        assert_eq!(sp.uv_min(), Vec2::new(0.75, 0.75));
        assert_eq!(sp.uv_max(), Vec2::new(1.0, 1.0));
    }

    #[test]
    fn test_sprite_bounding_box() {
        let sp = Sprite::new(TextureId::new(0), 64.0, 64.0);
        let (w, h) = sp.bounding_box_size(1.0, 1.0);
        assert_eq!(w, 64.0);
        assert_eq!(h, 64.0);

        let (w, h) = sp.bounding_box_size(2.0, 2.0);
        assert_eq!(w, 128.0);
        assert_eq!(h, 128.0);
    }

    #[test]
    fn test_sprite_aspect_ratio() {
        let sp1 = Sprite::new(TextureId::new(0), 128.0, 64.0);
        assert!((sp1.aspect_ratio() - 2.0).abs() < 0.01);

        let sp2 = Sprite::new(TextureId::new(0), 64.0, 128.0);
        assert!((sp2.aspect_ratio() - 0.5).abs() < 0.01);
    }

    #[test]
    fn test_sprite_reset() {
        let mut sp = Sprite::new(TextureId::new(0), 64.0, 64.0);
        sp.set_color(0xFF0000FF);
        sp.set_uv(Vec2::new(0.25, 0.25), Vec2::new(0.75, 0.75));

        sp.reset();
        assert_eq!(sp.color(), 0xFFFFFFFF);
        assert_eq!(sp.uv_min(), Vec2::ZERO);
        assert_eq!(sp.uv_max(), Vec2::ONE);
    }

    #[test]
    fn test_sprite_to_string() {
        let sp = Sprite::new(TextureId::new(0), 64.0, 64.0);
        let s = sp.to_string();
        assert!(s.contains("Sprite"));
        assert!(s.contains("64"));
        assert!(s.contains("#FFFFFFFF"));
    }
}
