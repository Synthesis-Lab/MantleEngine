use glam::Vec2;

/// Collision şeklini tanımla: AABB veya Daire
#[derive(Clone, Debug, PartialEq)]
pub enum Collider {
    /// Axis-Aligned Bounding Box: x, y, width, height
    Aabb {
        x: f32,
        y: f32,
        width: f32,
        height: f32,
    },
    /// Daire: x, y, radius
    Circle { x: f32, y: f32, radius: f32 },
}

impl Collider {
    /// Yeni bir AABB collider oluştur
    pub fn aabb(x: f32, y: f32, width: f32, height: f32) -> Self {
        Collider::Aabb { x, y, width, height }
    }

    /// Yeni bir daire collider oluştur
    pub fn circle(x: f32, y: f32, radius: f32) -> Self {
        Collider::Circle { x, y, radius }
    }

    /// Collider'ın merkez noktasını al
    pub fn center(&self) -> Vec2 {
        match self {
            Collider::Aabb { x, y, width, height } => Vec2::new(x + width / 2.0, y + height / 2.0),
            Collider::Circle { x, y, .. } => Vec2::new(*x, *y),
        }
    }

    /// AABB collider'ı konumlandır
    pub fn set_aabb_position(&mut self, x: f32, y: f32) {
        match self {
            Collider::Aabb {
                x: ref mut cx,
                y: ref mut cy,
                ..
            } => {
                *cx = x;
                *cy = y;
            }
            _ => {}
        }
    }

    /// Daire collider'ı konumlandır
    pub fn set_circle_position(&mut self, x: f32, y: f32) {
        match self {
            Collider::Circle {
                x: ref mut cx,
                y: ref mut cy,
                ..
            } => {
                *cx = x;
                *cy = y;
            }
            _ => {}
        }
    }

    /// AABB boyutunu değiştir
    pub fn set_aabb_size(&mut self, width: f32, height: f32) {
        match self {
            Collider::Aabb {
                width: ref mut w,
                height: ref mut h,
                ..
            } => {
                *w = width;
                *h = height;
            }
            _ => {}
        }
    }

    /// Daire yarıçapını değiştir
    pub fn set_circle_radius(&mut self, radius: f32) {
        match self {
            Collider::Circle { radius: ref mut r, .. } => *r = radius,
            _ => {}
        }
    }

    /// AABB için sınır değerlerini al (left, right, top, bottom)
    pub fn aabb_bounds(&self) -> Option<(f32, f32, f32, f32)> {
        match self {
            Collider::Aabb { x, y, width, height } => {
                Some((*x, x + width, *y, y + height))
            }
            _ => None,
        }
    }

    /// String gösterimi
    pub fn to_string(&self) -> String {
        match self {
            Collider::Aabb { x, y, width, height } => {
                format!("Collider::Aabb(x={}, y={}, w={}, h={})", x, y, width, height)
            }
            Collider::Circle { x, y, radius } => {
                format!("Collider::Circle(x={}, y={}, r={})", x, y, radius)
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_aabb_creation() {
        let collider = Collider::aabb(10.0, 20.0, 64.0, 64.0);
        match collider {
            Collider::Aabb { x, y, width, height } => {
                assert_eq!(x, 10.0);
                assert_eq!(y, 20.0);
                assert_eq!(width, 64.0);
                assert_eq!(height, 64.0);
            }
            _ => panic!("Expected AABB collider"),
        }
    }

    #[test]
    fn test_circle_creation() {
        let collider = Collider::circle(50.0, 50.0, 25.0);
        match collider {
            Collider::Circle { x, y, radius } => {
                assert_eq!(x, 50.0);
                assert_eq!(y, 50.0);
                assert_eq!(radius, 25.0);
            }
            _ => panic!("Expected Circle collider"),
        }
    }

    #[test]
    fn test_aabb_center() {
        let collider = Collider::aabb(10.0, 20.0, 100.0, 80.0);
        let center = collider.center();
        assert_eq!(center.x, 60.0);
        assert_eq!(center.y, 60.0);
    }

    #[test]
    fn test_circle_center() {
        let collider = Collider::circle(100.0, 200.0, 50.0);
        let center = collider.center();
        assert_eq!(center.x, 100.0);
        assert_eq!(center.y, 200.0);
    }

    #[test]
    fn test_set_aabb_position() {
        let mut collider = Collider::aabb(10.0, 20.0, 64.0, 64.0);
        collider.set_aabb_position(50.0, 60.0);
        match collider {
            Collider::Aabb { x, y, .. } => {
                assert_eq!(x, 50.0);
                assert_eq!(y, 60.0);
            }
            _ => panic!("Expected AABB collider"),
        }
    }

    #[test]
    fn test_set_circle_position() {
        let mut collider = Collider::circle(100.0, 200.0, 50.0);
        collider.set_circle_position(150.0, 250.0);
        match collider {
            Collider::Circle { x, y, .. } => {
                assert_eq!(x, 150.0);
                assert_eq!(y, 250.0);
            }
            _ => panic!("Expected Circle collider"),
        }
    }

    #[test]
    fn test_set_aabb_size() {
        let mut collider = Collider::aabb(10.0, 20.0, 64.0, 64.0);
        collider.set_aabb_size(128.0, 96.0);
        match collider {
            Collider::Aabb { width, height, .. } => {
                assert_eq!(width, 128.0);
                assert_eq!(height, 96.0);
            }
            _ => panic!("Expected AABB collider"),
        }
    }

    #[test]
    fn test_set_circle_radius() {
        let mut collider = Collider::circle(100.0, 200.0, 50.0);
        collider.set_circle_radius(75.0);
        match collider {
            Collider::Circle { radius, .. } => {
                assert_eq!(radius, 75.0);
            }
            _ => panic!("Expected Circle collider"),
        }
    }

    #[test]
    fn test_aabb_bounds() {
        let collider = Collider::aabb(10.0, 20.0, 100.0, 80.0);
        let bounds = collider.aabb_bounds();
        assert!(bounds.is_some());
        let (left, right, top, bottom) = bounds.unwrap();
        assert_eq!(left, 10.0);
        assert_eq!(right, 110.0);
        assert_eq!(top, 20.0);
        assert_eq!(bottom, 100.0);
    }

    #[test]
    fn test_collider_equality() {
        let c1 = Collider::aabb(10.0, 20.0, 64.0, 64.0);
        let c2 = Collider::aabb(10.0, 20.0, 64.0, 64.0);
        assert_eq!(c1, c2);
    }

    #[test]
    fn test_collider_to_string() {
        let aabb = Collider::aabb(10.0, 20.0, 64.0, 64.0);
        let s = aabb.to_string();
        assert!(s.contains("Collider::Aabb"));

        let circle = Collider::circle(50.0, 50.0, 25.0);
        let s = circle.to_string();
        assert!(s.contains("Collider::Circle"));
    }
}
