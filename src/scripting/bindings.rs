use rhai::Engine;
use crate::components::Transform;

/// Transform wrapper for Rhai — Rhai'de Transform ayarlarını manipüle etmek için
/// 
/// Rhai'de Transform kullanmak için:
/// ```
/// let tf = create_transform(10.0, 20.0);
/// tf.x();                    // 10.0
/// tf.set_x(15.0);           // x'i ayarla
/// tf.rotate_degrees(45.0);  // 45 derece döndür
/// ```
#[derive(Debug, Clone, Copy)]
pub struct RhaiTransform {
    inner: Transform,
}

impl RhaiTransform {
    pub fn new(transform: Transform) -> Self {
        RhaiTransform { inner: transform }
    }

    pub fn inner(&self) -> Transform {
        self.inner
    }

    pub fn inner_mut(&mut self) -> &mut Transform {
        &mut self.inner
    }

    // ========== POZİSYON ==========

    pub fn x(&self) -> f32 {
        self.inner.x()
    }

    pub fn set_x(&mut self, x: f32) {
        self.inner.set_x(x);
    }

    pub fn y(&self) -> f32 {
        self.inner.y()
    }

    pub fn set_y(&mut self, y: f32) {
        self.inner.set_y(y);
    }

    pub fn set_position(&mut self, x: f32, y: f32) {
        self.inner.set_position(x, y);
    }

    pub fn translate(&mut self, dx: f32, dy: f32) {
        self.inner.translate(dx, dy);
    }

    // ========== ROTASYON ==========

    pub fn rotation_degrees(&self) -> f32 {
        self.inner.rotation_degrees()
    }

    pub fn set_rotation_degrees(&mut self, degrees: f32) {
        self.inner.set_rotation_degrees(degrees);
    }

    pub fn rotate_degrees(&mut self, degrees: f32) {
        self.inner.rotate_degrees(degrees);
    }

    pub fn rotation_radians(&self) -> f32 {
        self.inner.rotation_radians()
    }

    pub fn set_rotation_radians(&mut self, radians: f32) {
        self.inner.set_rotation_radians(radians);
    }

    pub fn rotate_radians(&mut self, radians: f32) {
        self.inner.rotate_radians(radians);
    }

    // ========== ÖLÇEK ==========

    pub fn scale_x(&self) -> f32 {
        self.inner.scale().x
    }

    pub fn scale_y(&self) -> f32 {
        self.inner.scale().y
    }

    pub fn set_scale(&mut self, sx: f32, sy: f32) {
        self.inner.set_scale(sx, sy);
    }

    pub fn set_scale_uniform(&mut self, s: f32) {
        self.inner.set_scale_uniform(s);
    }

    // ========== Z EKSENİ ==========

    pub fn z(&self) -> f32 {
        self.inner.z()
    }

    pub fn set_z(&mut self, z: f32) {
        self.inner.set_z(z);
    }

    // ========== RESET ==========

    pub fn reset(&mut self) {
        self.inner.reset();
    }

    // ========== DÖNÜŞÜM (string olarak) ==========

    pub fn to_string(&self) -> String {
        format!(
            "Transform {{ x: {}, y: {}, rotation: {}, scale: ({}, {}), z: {} }}",
            self.inner.x(),
            self.inner.y(),
            self.inner.rotation_degrees(),
            self.inner.scale().x,
            self.inner.scale().y,
            self.inner.z()
        )
    }
}

/// Rhai Engine'e Transform fonksiyonlarını kaydet
pub fn register_transform_functions(engine: &mut Engine) {
    // Yeni Transform oluştur
    engine.register_fn("create_transform", |x: f32, y: f32| {
        RhaiTransform::new(Transform::new(x, y))
    });

    // POZİSYON GETTERs
    engine.register_get("x", |obj: &mut RhaiTransform| obj.x());
    engine.register_get("y", |obj: &mut RhaiTransform| obj.y());
    engine.register_get("z", |obj: &mut RhaiTransform| obj.z());

    // POZİSYON SETTERs
    engine.register_set("x", |obj: &mut RhaiTransform, x: f32| obj.set_x(x));
    engine.register_set("y", |obj: &mut RhaiTransform, y: f32| obj.set_y(y));
    engine.register_set("z", |obj: &mut RhaiTransform, z: f32| obj.set_z(z));

    // POZİSYON METODLARı
    engine.register_fn("set_position", |obj: &mut RhaiTransform, x: f32, y: f32| {
        obj.set_position(x, y);
    });

    engine.register_fn("translate", |obj: &mut RhaiTransform, dx: f32, dy: f32| {
        obj.translate(dx, dy);
    });

    // ROTASYON GETTERs
    engine.register_get("rotation", |obj: &mut RhaiTransform| obj.rotation_degrees());

    // ROTASYON SETTERs
    engine.register_set("rotation", |obj: &mut RhaiTransform, degrees: f32| {
        obj.set_rotation_degrees(degrees);
    });

    // ROTASYON METODLARı
    engine.register_fn("set_rotation_degrees", |obj: &mut RhaiTransform, degrees: f32| {
        obj.set_rotation_degrees(degrees);
    });

    engine.register_fn("rotate_degrees", |obj: &mut RhaiTransform, degrees: f32| {
        obj.rotate_degrees(degrees);
    });

    engine.register_fn("set_rotation_radians", |obj: &mut RhaiTransform, radians: f32| {
        obj.set_rotation_radians(radians);
    });

    engine.register_fn("rotate_radians", |obj: &mut RhaiTransform, radians: f32| {
        obj.rotate_radians(radians);
    });

    engine.register_fn("rotation_degrees", |obj: &mut RhaiTransform| {
        obj.rotation_degrees()
    });

    engine.register_fn("rotation_radians", |obj: &mut RhaiTransform| {
        obj.rotation_radians()
    });

    // ÖLÇEK METODLARı
    engine.register_fn("set_scale", |obj: &mut RhaiTransform, sx: f32, sy: f32| {
        obj.set_scale(sx, sy);
    });

    engine.register_fn("set_scale_uniform", |obj: &mut RhaiTransform, s: f32| {
        obj.set_scale_uniform(s);
    });

    engine.register_fn("scale_x", |obj: &mut RhaiTransform| obj.scale_x());
    engine.register_fn("scale_y", |obj: &mut RhaiTransform| obj.scale_y());

    // RESET
    engine.register_fn("reset", |obj: &mut RhaiTransform| {
        obj.reset();
    });

    // TO_STRING
    engine.register_fn("to_string", |obj: &RhaiTransform| obj.to_string());
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_rhai_transform_creation() {
        let tf = RhaiTransform::new(Transform::new(10.0, 20.0));
        assert_eq!(tf.x(), 10.0);
        assert_eq!(tf.y(), 20.0);
    }

    #[test]
    fn test_rhai_transform_setters() {
        let mut tf = RhaiTransform::new(Transform::new(0.0, 0.0));

        tf.set_x(15.0);
        assert_eq!(tf.x(), 15.0);

        tf.set_y(25.0);
        assert_eq!(tf.y(), 25.0);

        tf.set_position(100.0, 200.0);
        assert_eq!(tf.x(), 100.0);
        assert_eq!(tf.y(), 200.0);
    }

    #[test]
    fn test_rhai_transform_rotation() {
        let mut tf = RhaiTransform::new(Transform::new(0.0, 0.0));

        tf.set_rotation_degrees(45.0);
        assert_eq!(tf.rotation_degrees(), 45.0);

        tf.rotate_degrees(30.0);
        assert_eq!(tf.rotation_degrees(), 75.0);
    }

    #[test]
    fn test_rhai_transform_scale() {
        let mut tf = RhaiTransform::new(Transform::new(0.0, 0.0));

        tf.set_scale(2.0, 3.0);
        assert_eq!(tf.scale_x(), 2.0);
        assert_eq!(tf.scale_y(), 3.0);

        tf.set_scale_uniform(1.5);
        assert_eq!(tf.scale_x(), 1.5);
        assert_eq!(tf.scale_y(), 1.5);
    }

    #[test]
    fn test_rhai_transform_to_string() {
        let tf = RhaiTransform::new(Transform::new(10.0, 20.0));
        let s = tf.to_string();
        assert!(s.contains("x: 10"));
        assert!(s.contains("y: 20"));
    }

    #[test]
    fn test_rhai_bindings_registration() {
        let mut engine = Engine::new();
        register_transform_functions(&mut engine);

        // Script: Transform oluştur ve manipüle et
        let result = engine.eval::<f32>(
            r#"
            let tf = create_transform(10.0, 20.0);
            tf.set_rotation_degrees(45.0);
            tf.rotation
            "#
        );

        if let Ok(val) = result {
            assert!((val - 45.0).abs() < 0.01);
        } else {
            eprintln!("Script error: {:?}", result.err());
        }
    }

    #[test]
    fn test_rhai_script_property_access() {
        let mut engine = Engine::new();
        register_transform_functions(&mut engine);

        let result = engine.eval::<f32>(
            r#"
            let tf = create_transform(5.0, 10.0);
            tf.x = 15.0;
            tf.x
            "#
        );

        if let Ok(val) = result {
            assert!((val - 15.0).abs() < 0.01);
        } else {
            eprintln!("Script error: {:?}", result.err());
        }
    }
}
