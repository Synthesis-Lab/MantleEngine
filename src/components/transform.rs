use glam::Vec2;
use std::f32::consts::PI;
use bevy_ecs::component::Component;

/// 2D Transform Component — Konum, Rotasyon ve Ölçekleme
/// 
/// # Notlar
/// - Rotasyon: Euler açı cinsinden (derece)
/// - Ölçek: Vec2 (2D), 3D geçişine hazırlık için z alanı reserved
/// - Z eksenel 3D desteği için şimdiden struct'ta yer alıyor ama şimdilik "reserved"
#[derive(Debug, Clone, Copy, PartialEq, Component)]
pub struct Transform {
    /// 2D Konum (x, y)
    position: Vec2,
    /// Rotasyon (derece cinsinden). 0-360 arasında
    rotation_degrees: f32,
    /// 2D Ölçekleme (x, y)
    scale: Vec2,
    /// Z eksenel (3D geçişine hazırlık, şimdi reserved)
    z: f32,
}

impl Transform {
    /// Yeni bir Transform oluştur
    ///
    /// # Örnek
    /// ```
    /// use mantle_engine::Transform;
    /// let tf = Transform::new(10.0, 20.0);
    /// ```
    pub fn new(x: f32, y: f32) -> Self {
        Self {
            position: Vec2::new(x, y),
            rotation_degrees: 0.0,
            scale: Vec2::ONE,
            z: 0.0,
        }
    }

    /// Varsayılan Transform (origin'de, rotasyonsuz, normal ölçekte)
    pub fn default() -> Self {
        Self {
            position: Vec2::ZERO,
            rotation_degrees: 0.0,
            scale: Vec2::ONE,
            z: 0.0,
        }
    }

    // ========== POZİSYON İŞLEMLERİ ==========

    /// Pozisyonu al
    pub fn position(&self) -> Vec2 {
        self.position
    }

    /// Pozisyonu ayarla
    pub fn set_position(&mut self, x: f32, y: f32) {
        self.position = Vec2::new(x, y);
    }

    /// Konum vektörünü ayarla
    pub fn set_position_vec(&mut self, pos: Vec2) {
        self.position = pos;
    }

    /// X konumunu al
    pub fn x(&self) -> f32 {
        self.position.x
    }

    /// Y konumunu al
    pub fn y(&self) -> f32 {
        self.position.y
    }

    /// X konumunu ayarla
    pub fn set_x(&mut self, x: f32) {
        self.position.x = x;
    }

    /// Y konumunu ayarla
    pub fn set_y(&mut self, y: f32) {
        self.position.y = y;
    }

    /// Konum üzerinde öteleme (translation). Geçerli konuma ekle
    pub fn translate(&mut self, dx: f32, dy: f32) {
        self.position += Vec2::new(dx, dy);
    }

    /// Konum vektörü ile öteleme
    pub fn translate_vec(&mut self, delta: Vec2) {
        self.position += delta;
    }

    // ========== ROTASYON İŞLEMLERİ ==========

    /// Rotasyonu derece cinsinden al
    pub fn rotation_degrees(&self) -> f32 {
        self.rotation_degrees
    }

    /// Rotasyonu derece cinsinden ayarla
    pub fn set_rotation_degrees(&mut self, degrees: f32) {
        self.rotation_degrees = degrees % 360.0;
    }

    /// Rotasyonu radian cinsinden al
    pub fn rotation_radians(&self) -> f32 {
        self.rotation_degrees * PI / 180.0
    }

    /// Rotasyonu radian cinsinden ayarla
    pub fn set_rotation_radians(&mut self, radians: f32) {
        self.rotation_degrees = (radians * 180.0 / PI) % 360.0;
    }

    /// Rotasyona ekleme (derece)
    pub fn rotate_degrees(&mut self, degrees: f32) {
        self.rotation_degrees = (self.rotation_degrees + degrees) % 360.0;
    }

    /// Rotasyona ekleme (radian)
    pub fn rotate_radians(&mut self, radians: f32) {
        let degrees = radians * 180.0 / PI;
        self.rotate_degrees(degrees);
    }

    // ========== ÖLÇEK İŞLEMLERİ ==========

    /// Ölçeklemeyi al
    pub fn scale(&self) -> Vec2 {
        self.scale
    }

    /// Ölçeklemeyi ayarla (x, y)
    pub fn set_scale(&mut self, sx: f32, sy: f32) {
        self.scale = Vec2::new(sx, sy);
    }

    /// Ölçek vektörünü ayarla
    pub fn set_scale_vec(&mut self, s: Vec2) {
        self.scale = s;
    }

    /// Ölçeklemeyi tek bir değerle ayarla (uniform scaling)
    pub fn set_scale_uniform(&mut self, s: f32) {
        self.scale = Vec2::splat(s);
    }

    // ========== Z EKSENİ (3D Hazırlığı) ==========

    /// Z konumunu al (şimdilik reserved, 0.0 olmalı)
    pub fn z(&self) -> f32 {
        self.z
    }

    /// Z konumunu ayarla (3D geçişine hazırlık)
    pub fn set_z(&mut self, z: f32) {
        self.z = z;
    }

    // ========== KÜTÜPHANE METODLARI ==========

    /// Transform'u sıfırla (identity transform)
    pub fn reset(&mut self) {
        self.position = Vec2::ZERO;
        self.rotation_degrees = 0.0;
        self.scale = Vec2::ONE;
        self.z = 0.0;
    }

    /// Rotasyon matrisini (2D) hesapla (gelecek: 2D fizik için)
    pub fn rotation_matrix_2d(&self) -> [[f32; 2]; 2] {
        let c = self.rotation_radians().cos();
        let s = self.rotation_radians().sin();
        [
            [c, -s],
            [s, c],
        ]
    }
}

impl Default for Transform {
    fn default() -> Self {
        Self::default()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_transform_new() {
        let tf = Transform::new(5.0, 10.0);
        assert_eq!(tf.x(), 5.0);
        assert_eq!(tf.y(), 10.0);
        assert_eq!(tf.rotation_degrees(), 0.0);
        assert_eq!(tf.scale(), Vec2::ONE);
    }

    #[test]
    fn test_transform_default() {
        let tf = Transform::default();
        assert_eq!(tf.position(), Vec2::ZERO);
        assert_eq!(tf.rotation_degrees(), 0.0);
        assert_eq!(tf.scale(), Vec2::ONE);
    }

    #[test]
    fn test_position_operations() {
        let mut tf = Transform::new(0.0, 0.0);
        
        tf.set_position(10.0, 20.0);
        assert_eq!(tf.x(), 10.0);
        assert_eq!(tf.y(), 20.0);
        
        tf.set_x(15.0);
        assert_eq!(tf.x(), 15.0);
        
        tf.set_y(25.0);
        assert_eq!(tf.y(), 25.0);
    }

    #[test]
    fn test_translation() {
        let mut tf = Transform::new(10.0, 20.0);
        tf.translate(5.0, 3.0);
        
        assert_eq!(tf.x(), 15.0);
        assert_eq!(tf.y(), 23.0);
    }

    #[test]
    fn test_rotation_degrees() {
        let mut tf = Transform::new(0.0, 0.0);
        
        tf.set_rotation_degrees(45.0);
        assert_eq!(tf.rotation_degrees(), 45.0);
        
        tf.rotate_degrees(30.0);
        assert_eq!(tf.rotation_degrees(), 75.0);
    }

    #[test]
    fn test_rotation_degrees_wrapping() {
        let mut tf = Transform::new(0.0, 0.0);
        tf.set_rotation_degrees(370.0);
        
        // 370 % 360 = 10
        assert_eq!(tf.rotation_degrees(), 10.0);
    }

    #[test]
    fn test_rotation_radians_conversion() {
        let mut tf = Transform::new(0.0, 0.0);
        
        // 90 derece = π/2 radian
        tf.set_rotation_degrees(90.0);
        let rad = tf.rotation_radians();
        assert!((rad - PI / 2.0).abs() < 0.0001);
    }

    #[test]
    fn test_set_rotation_radians() {
        let mut tf = Transform::new(0.0, 0.0);
        
        // π/4 radian = 45 derece
        tf.set_rotation_radians(PI / 4.0);
        assert!((tf.rotation_degrees() - 45.0).abs() < 0.0001);
    }

    #[test]
    fn test_scale_operations() {
        let mut tf = Transform::new(0.0, 0.0);
        
        tf.set_scale(2.0, 3.0);
        assert_eq!(tf.scale(), Vec2::new(2.0, 3.0));
        
        tf.set_scale_uniform(1.5);
        assert_eq!(tf.scale(), Vec2::new(1.5, 1.5));
    }

    #[test]
    fn test_reset() {
        let mut tf = Transform::new(10.0, 20.0);
        tf.set_rotation_degrees(45.0);
        tf.set_scale(2.0, 3.0);
        tf.set_z(5.0);
        
        tf.reset();
        
        assert_eq!(tf.position(), Vec2::ZERO);
        assert_eq!(tf.rotation_degrees(), 0.0);
        assert_eq!(tf.scale(), Vec2::ONE);
        assert_eq!(tf.z(), 0.0);
    }

    #[test]
    fn test_z_operations() {
        let mut tf = Transform::new(0.0, 0.0);
        tf.set_z(10.0);
        assert_eq!(tf.z(), 10.0);
    }

    #[test]
    fn test_rotation_matrix_2d() {
        let mut tf = Transform::new(0.0, 0.0);
        tf.set_rotation_degrees(0.0);
        
        let mat = tf.rotation_matrix_2d();
        // 0 derece = identity matrix
        assert!(mat[0][0] > 0.9999 && mat[0][0] < 1.0001);
        assert!(mat[0][1].abs() < 0.0001);
        assert!(mat[1][0].abs() < 0.0001);
        assert!(mat[1][1] > 0.9999 && mat[1][1] < 1.0001);
    }
}
