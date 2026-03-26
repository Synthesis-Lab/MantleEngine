use rhai::Engine;
use glam::Vec2;
use crate::components::{Transform, Sprite, Collider, Animation, AnimationFrame, AnimationState};
use crate::assets::TextureId;
use crate::collision::check_collision;

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

// ============= SPRITE BINDINGS =============

/// Sprite wrapper for Rhai — Rhai'de Sprite ayarlarını manipüle etmek için
#[derive(Debug, Clone, Copy)]
pub struct RhaiSprite {
    inner: Sprite,
}

impl RhaiSprite {
    pub fn new(sprite: Sprite) -> Self {
        RhaiSprite { inner: sprite }
    }

    pub fn inner(&self) -> Sprite {
        self.inner
    }

    pub fn inner_mut(&mut self) -> &mut Sprite {
        &mut self.inner
    }

    // ========== TEXTURE ==========

    pub fn texture_id(&self) -> u32 {
        self.inner.texture_id().as_u32()
    }

    pub fn set_texture_id(&mut self, id: u32) {
        self.inner.set_texture_id(TextureId::new(id));
    }

    pub fn width(&self) -> f32 {
        self.inner.width()
    }

    pub fn height(&self) -> f32 {
        self.inner.height()
    }

    pub fn set_size(&mut self, width: f32, height: f32) {
        self.inner.set_size(width, height);
    }

    // ========== RENK ==========

    pub fn color(&self) -> u32 {
        self.inner.color()
    }

    pub fn set_color(&mut self, color: u32) {
        self.inner.set_color(color);
    }

    pub fn alpha(&self) -> u32 {
        self.inner.alpha() as u32
    }

    pub fn set_alpha(&mut self, alpha: u32) {
        self.inner.set_alpha(alpha as u8);
    }

    pub fn color_hex(&self) -> String {
        self.inner.color_hex()
    }

    pub fn set_color_hex(&mut self, hex: &str) -> bool {
        self.inner.set_color_hex(hex).is_ok()
    }

    // ========== UV ==========

    pub fn uv_min_x(&self) -> f32 {
        self.inner.uv_min().x
    }

    pub fn uv_min_y(&self) -> f32 {
        self.inner.uv_min().y
    }

    pub fn uv_max_x(&self) -> f32 {
        self.inner.uv_max().x
    }

    pub fn uv_max_y(&self) -> f32 {
        self.inner.uv_max().y
    }

    pub fn set_uv(&mut self, min_x: f32, min_y: f32, max_x: f32, max_y: f32) {
        self.inner
            .set_uv(Vec2::new(min_x, min_y), Vec2::new(max_x, max_y));
    }

    pub fn set_uv_rect(&mut self, x: f32, y: f32, w: f32, h: f32) {
        self.inner.set_uv_rect(x, y, w, h);
    }

    // ========== DIĞER ==========

    pub fn reset(&mut self) {
        self.inner.reset();
    }

    pub fn to_string(&self) -> String {
        self.inner.to_string()
    }
}

/// Rhai Engine'e Sprite fonksiyonlarını kaydet
pub fn register_sprite_functions(engine: &mut Engine) {
    // Yeni Sprite oluştur
    engine.register_fn("create_sprite", |texture_id: u32, width: f32, height: f32| {
        RhaiSprite::new(Sprite::new(TextureId::new(texture_id), width, height))
    });

    // TEXTURE GETTERs/SETTERs
    engine.register_get("texture_id", |obj: &mut RhaiSprite| obj.texture_id());
    engine.register_set("texture_id", |obj: &mut RhaiSprite, id: u32| {
        obj.set_texture_id(id);
    });

    engine.register_fn("width", |obj: &mut RhaiSprite| obj.width());
    engine.register_fn("height", |obj: &mut RhaiSprite| obj.height());

    engine.register_fn("set_size", |obj: &mut RhaiSprite, w: f32, h: f32| {
        obj.set_size(w, h);
    });

    // RENK GETTERs/SETTERs
    engine.register_get("color", |obj: &mut RhaiSprite| obj.color());
    engine.register_set("color", |obj: &mut RhaiSprite, c: u32| obj.set_color(c));

    engine.register_fn("alpha", |obj: &mut RhaiSprite| obj.alpha());
    engine.register_fn("set_alpha", |obj: &mut RhaiSprite, a: u32| {
        obj.set_alpha(a);
    });

    engine.register_fn("color_hex", |obj: &RhaiSprite| obj.color_hex());
    engine.register_fn("set_color_hex", |obj: &mut RhaiSprite, hex: &str| {
        obj.set_color_hex(hex)
    });

    // UV METODLARı
    engine.register_fn(
        "set_uv",
        |obj: &mut RhaiSprite, min_x: f32, min_y: f32, max_x: f32, max_y: f32| {
            obj.set_uv(min_x, min_y, max_x, max_y);
        },
    );

    engine.register_fn(
        "set_uv_rect",
        |obj: &mut RhaiSprite, x: f32, y: f32, w: f32, h: f32| {
            obj.set_uv_rect(x, y, w, h);
        },
    );

    // RESET ve TO_STRING
    engine.register_fn("reset", |obj: &mut RhaiSprite| {
        obj.reset();
    });

    engine.register_fn("to_string", |obj: &RhaiSprite| obj.to_string());
}

#[cfg(test)]
mod sprite_tests {
    use super::*;

    #[test]
    fn test_rhai_sprite_creation() {
        let sp = RhaiSprite::new(Sprite::new(TextureId::new(0), 64.0, 64.0));
        assert_eq!(sp.texture_id(), 0);
        assert_eq!(sp.width(), 64.0);
        assert_eq!(sp.height(), 64.0);
    }

    #[test]
    fn test_rhai_sprite_color() {
        let mut sp = RhaiSprite::new(Sprite::new(TextureId::new(0), 64.0, 64.0));
        assert_eq!(sp.color(), 0xFFFFFFFF);

        sp.set_color(0xFF0000FF);
        assert_eq!(sp.color(), 0xFF0000FF);
    }

    #[test]
    fn test_rhai_sprite_bindings_registration() {
        let mut engine = Engine::new();
        register_sprite_functions(&mut engine);

        let result = engine.eval::<u32>(
            r#"
            let sp = create_sprite(0, 64.0, 64.0);
            sp.color = 0xFF0000FF;
            sp.color
            "#
        );

        if let Ok(val) = result {
            assert_eq!(val, 0xFF0000FF);
        } else {
            eprintln!("Script error: {:?}", result.err());
        }
    }

    #[test]
    fn test_rhai_sprite_set_size() {
        let mut engine = Engine::new();
        register_sprite_functions(&mut engine);

        let result = engine.eval::<f32>(
            r#"
            let sp = create_sprite(0, 64.0, 64.0);
            sp.set_size(128.0, 256.0);
            sp.width()
            "#
        );

        if let Ok(val) = result {
            assert!((val - 128.0).abs() < 0.01);
        } else {
            eprintln!("Script error: {:?}", result.err());
        }
    }
}

/// Collider wrapper for Rhai — Çarpışma kontrolü için
#[derive(Debug, Clone)]
pub struct RhaiCollider {
    inner: Collider,
}

impl RhaiCollider {
    pub fn new(collider: Collider) -> Self {
        RhaiCollider { inner: collider }
    }

    pub fn inner(&self) -> &Collider {
        &self.inner
    }

    pub fn inner_mut(&mut self) -> &mut Collider {
        &mut self.inner
    }

    // ========== AABB METODLARI ==========

    pub fn set_aabb_position(&mut self, x: f32, y: f32) {
        self.inner.set_aabb_position(x, y);
    }

    pub fn set_aabb_size(&mut self, width: f32, height: f32) {
        self.inner.set_aabb_size(width, height);
    }

    // ========== DAIRE METODLARI ==========

    pub fn set_circle_position(&mut self, x: f32, y: f32) {
        self.inner.set_circle_position(x, y);
    }

    pub fn set_circle_radius(&mut self, radius: f32) {
        self.inner.set_circle_radius(radius);
    }

    // ========== MERKEZ NOKTA ==========

    pub fn center_x(&self) -> f32 {
        self.inner.center().x
    }

    pub fn center_y(&self) -> f32 {
        self.inner.center().y
    }

    // ========== DIĞER ==========

    pub fn to_string(&self) -> String {
        self.inner.to_string()
    }
}

/// Rhai Engine'e Collision fonksiyonlarını kaydet
pub fn register_collision_functions(engine: &mut Engine) {
    // AABB Collider oluştur
    engine.register_fn("create_aabb", |x: f32, y: f32, width: f32, height: f32| {
        RhaiCollider::new(Collider::aabb(x, y, width, height))
    });

    // Daire Collider oluştur
    engine.register_fn("create_circle", |x: f32, y: f32, radius: f32| {
        RhaiCollider::new(Collider::circle(x, y, radius))
    });

    // Çarpışma kontrolü
    engine.register_fn("check_collision", |c1: RhaiCollider, c2: RhaiCollider| {
        check_collision(c1.inner(), c2.inner())
    });

    // AABB Metodları
    engine.register_fn(
        "set_aabb_position",
        |c: &mut RhaiCollider, x: f32, y: f32| {
            c.set_aabb_position(x, y);
        },
    );

    engine.register_fn(
        "set_aabb_size",
        |c: &mut RhaiCollider, width: f32, height: f32| {
            c.set_aabb_size(width, height);
        },
    );

    // Daire Metodları
    engine.register_fn("set_circle_position", |c: &mut RhaiCollider, x: f32, y: f32| {
        c.set_circle_position(x, y);
    });

    engine.register_fn("set_circle_radius", |c: &mut RhaiCollider, radius: f32| {
        c.set_circle_radius(radius);
    });

    // Merkez Noktası
    engine.register_fn("center_x", |c: &RhaiCollider| c.center_x());
    engine.register_fn("center_y", |c: &RhaiCollider| c.center_y());

    // String Gösterimi
    engine.register_fn("to_string", |c: &RhaiCollider| c.to_string());
}

#[cfg(test)]
mod collision_tests {
    use super::*;

    #[test]
    fn test_rhai_collider_aabb_creation() {
        let collider = RhaiCollider::new(Collider::aabb(10.0, 20.0, 64.0, 64.0));
        assert_eq!(collider.center_x(), 42.0);
        assert_eq!(collider.center_y(), 52.0);
    }

    #[test]
    fn test_rhai_collider_circle_creation() {
        let collider = RhaiCollider::new(Collider::circle(50.0, 50.0, 25.0));
        assert_eq!(collider.center_x(), 50.0);
        assert_eq!(collider.center_y(), 50.0);
    }

    #[test]
    fn test_rhai_collision_aabb_aabb() {
        let mut engine = Engine::new();
        register_collision_functions(&mut engine);

        let result = engine.eval::<bool>(
            r#"
            let c1 = create_aabb(0.0, 0.0, 100.0, 100.0);
            let c2 = create_aabb(50.0, 50.0, 100.0, 100.0);
            check_collision(c1, c2)
            "#
        );

        if let Ok(val) = result {
            assert!(val);
        } else {
            eprintln!("Script error: {:?}", result.err());
        }
    }

    #[test]
    fn test_rhai_collision_circle_circle() {
        let mut engine = Engine::new();
        register_collision_functions(&mut engine);

        let result = engine.eval::<bool>(
            r#"
            let c1 = create_circle(0.0, 0.0, 50.0);
            let c2 = create_circle(75.0, 0.0, 50.0);
            check_collision(c1, c2)
            "#
        );

        if let Ok(val) = result {
            assert!(val);
        } else {
            eprintln!("Script error: {:?}", result.err());
        }
    }

    #[test]
    fn test_rhai_collision_aabb_circle() {
        let mut engine = Engine::new();
        register_collision_functions(&mut engine);

        let result = engine.eval::<bool>(
            r#"
            let aabb = create_aabb(0.0, 0.0, 100.0, 100.0);
            let circle = create_circle(50.0, 50.0, 30.0);
            check_collision(aabb, circle)
            "#
        );

        if let Ok(val) = result {
            assert!(val);
        } else {
            eprintln!("Script error: {:?}", result.err());
        }
    }
}

/// Rhai Animation wrapper — Rhai'de animasyon oluşturmak ve yönetmek için
#[derive(Debug, Clone)]
pub struct RhaiAnimation {
    inner: Animation,
}

impl RhaiAnimation {
    pub fn new(animation: Animation) -> Self {
        RhaiAnimation { inner: animation }
    }

    pub fn inner(&self) -> &Animation {
        &self.inner
    }

    pub fn inner_mut(&mut self) -> &mut Animation {
        &mut self.inner
    }

    // ========== FRAME METODLARI ==========

    pub fn frame_count(&self) -> usize {
        self.inner.frame_count()
    }

    pub fn add_frame(&mut self, min_x: f32, min_y: f32, max_x: f32, max_y: f32, duration: f32) {
        let frame = AnimationFrame::new(
            Vec2::new(min_x, min_y),
            Vec2::new(max_x, max_y),
            duration,
        );
        self.inner.add_frame(frame);
    }

    pub fn add_frame_rect(&mut self, x: f32, y: f32, w: f32, h: f32, duration: f32) {
        let frame = AnimationFrame::from_uv_rect(x, y, w, h, duration);
        self.inner.add_frame(frame);
    }

    // ========== ÖZELLIKLER ==========

    pub fn name(&self) -> String {
        self.inner.name.clone()
    }

    pub fn speed(&self) -> f32 {
        self.inner.speed
    }

    pub fn set_speed(&mut self, speed: f32) {
        self.inner.speed = speed;
    }

    pub fn looping(&self) -> bool {
        self.inner.looping
    }

    pub fn set_looping(&mut self, looping: bool) {
        self.inner.looping = looping;
    }

    pub fn total_duration(&self) -> f32 {
        self.inner.total_duration()
    }

    // ========== DIĞER ==========

    pub fn to_string(&self) -> String {
        self.inner.to_string()
    }
}

/// Rhai AnimationState wrapper — Animasyon oynatma durumunu kontrol et
#[derive(Debug, Clone)]
pub struct RhaiAnimationState {
    inner: AnimationState,
}

impl RhaiAnimationState {
    pub fn new() -> Self {
        RhaiAnimationState {
            inner: AnimationState::new(),
        }
    }

    pub fn inner(&self) -> &AnimationState {
        &self.inner
    }

    pub fn inner_mut(&mut self) -> &mut AnimationState {
        &mut self.inner
    }

    // ========== KONTROL ==========

    pub fn play(&mut self) {
        self.inner.play();
    }

    pub fn pause(&mut self) {
        self.inner.pause();
    }

    pub fn stop(&mut self) {
        self.inner.stop();
    }

    // ========== DURUM ==========

    pub fn frame_index(&self) -> usize {
        self.inner.frame_index
    }

    pub fn is_playing(&self) -> bool {
        self.inner.is_playing
    }

    pub fn elapsed_time(&self) -> f32 {
        self.inner.elapsed_time
    }

    // ========== GÜNCELLEME ==========

    pub fn update(&mut self, delta_time: f32, animation: &mut RhaiAnimation) {
        self.inner.update(delta_time, animation.inner_mut());
    }

    // ========== DIĞER ==========

    pub fn to_string(&self) -> String {
        self.inner.to_string()
    }
}

/// Rhai Engine'e Animation fonksiyonlarını kaydet
pub fn register_animation_functions(engine: &mut Engine) {
    // Yeni Animation oluştur
    engine.register_fn("create_animation", |name: &str, speed: f32, looping: bool| {
        RhaiAnimation::new(Animation::new(name.to_string(), speed, looping))
    });

    // FRAME METODLARI
    engine.register_fn(
        "add_frame",
        |anim: &mut RhaiAnimation, min_x: f32, min_y: f32, max_x: f32, max_y: f32, duration: f32| {
            anim.add_frame(min_x, min_y, max_x, max_y, duration);
        },
    );

    engine.register_fn(
        "add_frame_rect",
        |anim: &mut RhaiAnimation, x: f32, y: f32, w: f32, h: f32, duration: f32| {
            anim.add_frame_rect(x, y, w, h, duration);
        },
    );

    engine.register_fn("frame_count", |anim: &RhaiAnimation| anim.frame_count());

    // ÖZELLIKLER
    engine.register_fn("name", |anim: &RhaiAnimation| anim.name());

    engine.register_get("speed", |anim: &mut RhaiAnimation| anim.speed());
    engine.register_set("speed", |anim: &mut RhaiAnimation, s: f32| {
        anim.set_speed(s);
    });

    engine.register_get("looping", |anim: &mut RhaiAnimation| anim.looping());
    engine.register_set("looping", |anim: &mut RhaiAnimation, l: bool| {
        anim.set_looping(l);
    });

    engine.register_fn("total_duration", |anim: &RhaiAnimation| anim.total_duration());

    // AnimationState oluştur
    engine.register_fn("create_animation_state", || RhaiAnimationState::new());

    // KONTROL METODLARI
    engine.register_fn("play", |state: &mut RhaiAnimationState| {
        state.play();
    });

    engine.register_fn("pause", |state: &mut RhaiAnimationState| {
        state.pause();
    });

    engine.register_fn("stop", |state: &mut RhaiAnimationState| {
        state.stop();
    });

    // DURUM GETTER'LAR
    engine.register_fn("frame_index", |state: &RhaiAnimationState| state.frame_index());
    engine.register_fn("is_playing", |state: &RhaiAnimationState| state.is_playing());
    engine.register_fn("elapsed_time", |state: &RhaiAnimationState| state.elapsed_time());

    // STRING GÖSTERIMI
    engine.register_fn("to_string", |anim: &RhaiAnimation| anim.to_string());
    engine.register_fn("to_string", |state: &RhaiAnimationState| state.to_string());
}

#[cfg(test)]
mod animation_tests {
    use super::*;

    #[test]
    fn test_rhai_animation_creation() {
        let anim = RhaiAnimation::new(Animation::new("walk".to_string(), 1.0, true));
        assert_eq!(anim.name(), "walk");
        assert_eq!(anim.speed(), 1.0);
        assert!(anim.looping());
        assert_eq!(anim.frame_count(), 0);
    }

    #[test]
    fn test_rhai_animation_add_frame() {
        let mut anim = RhaiAnimation::new(Animation::new("run".to_string(), 1.0, false));
        anim.add_frame(0.0, 0.0, 0.25, 1.0, 0.1);
        anim.add_frame(0.25, 0.0, 0.5, 1.0, 0.1);

        assert_eq!(anim.frame_count(), 2);
    }

    #[test]
    fn test_rhai_animation_speed_property() {
        let mut anim = RhaiAnimation::new(Animation::new("test".to_string(), 1.0, false));
        anim.set_speed(2.0);
        assert_eq!(anim.speed(), 2.0);
    }

    #[test]
    fn test_rhai_animation_looping_property() {
        let mut anim = RhaiAnimation::new(Animation::new("test".to_string(), 1.0, true));
        anim.set_looping(false);
        assert!(!anim.looping());
    }

    #[test]
    fn test_rhai_animation_state_creation() {
        let state = RhaiAnimationState::new();
        assert_eq!(state.frame_index(), 0);
        assert!(!state.is_playing());
    }

    #[test]
    fn test_rhai_animation_state_control() {
        let mut state = RhaiAnimationState::new();

        state.play();
        assert!(state.is_playing());

        state.pause();
        assert!(!state.is_playing());

        state.play();
        state.stop();
        assert!(!state.is_playing());
        assert_eq!(state.frame_index(), 0);
    }

    #[test]
    fn test_rhai_animation_state_frame_tracking() {
        let anim = RhaiAnimation::new(Animation::new("test".to_string(), 1.0, true));

        let mut state = RhaiAnimationState::new();
        state.play();
        assert!(state.is_playing());
        assert_eq!(state.frame_index(), 0);
    }

    #[test]
    fn test_rhai_animation_via_script() {
        let mut engine = Engine::new();
        register_animation_functions(&mut engine);

        let result = engine.eval::<usize>(
            r#"
            let anim = create_animation("walk", 1.0, true);
            anim.add_frame(0.0, 0.0, 0.25, 1.0, 0.1);
            anim.add_frame(0.25, 0.0, 0.5, 1.0, 0.1);
            anim.add_frame(0.5, 0.0, 0.75, 1.0, 0.1);
            anim.frame_count()
            "#
        );

        if let Ok(count) = result {
            assert_eq!(count, 3);
        } else {
            eprintln!("Script error: {:?}", result.err());
        }
    }

    #[test]
    fn test_rhai_animation_state_playback() {
        let mut engine = Engine::new();
        register_animation_functions(&mut engine);

        let result = engine.eval::<bool>(
            r#"
            let anim = create_animation("idle", 1.0, false);
            anim.add_frame(0.0, 0.0, 1.0, 1.0, 0.2);

            let state = create_animation_state();
            state.play();
            state.is_playing()
            "#
        );

        if let Ok(is_playing) = result {
            assert!(is_playing);
        } else {
            eprintln!("Script error: {:?}", result.err());
        }
    }

    #[test]
    fn test_rhai_animation_properties() {
        let mut engine = Engine::new();
        register_animation_functions(&mut engine);

        let result = engine.eval::<f32>(
            r#"
            let anim = create_animation("fast", 2.0, true);
            anim.speed = 1.5;
            anim.speed
            "#
        );

        if let Ok(speed) = result {
            assert!((speed - 1.5).abs() < 0.01);
        } else {
            eprintln!("Script error: {:?}", result.err());
        }
    }
}
