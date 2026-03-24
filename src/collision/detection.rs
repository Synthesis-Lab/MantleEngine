use crate::components::Collider;

/// İki collider arasındaki çarpışmayı kontrol et
pub fn check_collision(a: &Collider, b: &Collider) -> bool {
    match (a, b) {
        // AABB - AABB
        (
            Collider::Aabb {
                x: x1,
                y: y1,
                width: w1,
                height: h1,
            },
            Collider::Aabb {
                x: x2,
                y: y2,
                width: w2,
                height: h2,
            },
        ) => aabb_aabb_collision(*x1, *y1, *w1, *h1, *x2, *y2, *w2, *h2),

        // Circle - Circle
        (
            Collider::Circle {
                x: x1,
                y: y1,
                radius: r1,
            },
            Collider::Circle {
                x: x2,
                y: y2,
                radius: r2,
            },
        ) => circle_circle_collision(*x1, *y1, *r1, *x2, *y2, *r2),

        // AABB - Circle
        (
            Collider::Aabb {
                x: x1,
                y: y1,
                width: w1,
                height: h1,
            },
            Collider::Circle {
                x: x2,
                y: y2,
                radius: r2,
            },
        ) => aabb_circle_collision(*x1, *y1, *w1, *h1, *x2, *y2, *r2),

        // Circle - AABB (sıra önemli değil)
        (
            Collider::Circle {
                x: x1,
                y: y1,
                radius: r1,
            },
            Collider::Aabb {
                x: x2,
                y: y2,
                width: w2,
                height: h2,
            },
        ) => aabb_circle_collision(*x2, *y2, *w2, *h2, *x1, *y1, *r1),
    }
}

/// İki AABB collider'ı arasında çarpışma kontrolü
fn aabb_aabb_collision(x1: f32, y1: f32, w1: f32, h1: f32, x2: f32, y2: f32, w2: f32, h2: f32) -> bool {
    let left1 = x1;
    let right1 = x1 + w1;
    let top1 = y1;
    let bottom1 = y1 + h1;

    let left2 = x2;
    let right2 = x2 + w2;
    let top2 = y2;
    let bottom2 = y2 + h2;

    left1 < right2 && right1 > left2 && top1 < bottom2 && bottom1 > top2
}

/// İki çember arasında çarpışma kontrolü
fn circle_circle_collision(x1: f32, y1: f32, r1: f32, x2: f32, y2: f32, r2: f32) -> bool {
    let dx = x2 - x1;
    let dy = y2 - y1;
    let distance_squared = dx * dx + dy * dy;
    let min_distance = r1 + r2;
    distance_squared < min_distance * min_distance
}

/// AABB ve çember arasında çarpışma kontrolü
fn aabb_circle_collision(x1: f32, y1: f32, w1: f32, h1: f32, cx: f32, cy: f32, cr: f32) -> bool {
    // Çemberin merkezi AABB'nin en yakın noktasını bul
    let closest_x = cx.clamp(x1, x1 + w1);
    let closest_y = cy.clamp(y1, y1 + h1);

    // Çemberin merkezi ile en yakın nokta arasındaki uzaklığı hesapla
    let dx = cx - closest_x;
    let dy = cy - closest_y;
    let distance_squared = dx * dx + dy * dy;

    distance_squared < cr * cr
}

/// Çarpışma bilgisini döndür: çarpışma var mı?
pub fn is_colliding(a: &Collider, b: &Collider) -> bool {
    check_collision(a, b)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_aabb_aabb_collision_true() {
        let c1 = Collider::aabb(0.0, 0.0, 100.0, 100.0);
        let c2 = Collider::aabb(50.0, 50.0, 100.0, 100.0);
        assert!(check_collision(&c1, &c2));
    }

    #[test]
    fn test_aabb_aabb_no_collision() {
        let c1 = Collider::aabb(0.0, 0.0, 100.0, 100.0);
        let c2 = Collider::aabb(200.0, 200.0, 100.0, 100.0);
        assert!(!check_collision(&c1, &c2));
    }

    #[test]
    fn test_aabb_aabb_touching_edges() {
        let c1 = Collider::aabb(0.0, 0.0, 100.0, 100.0);
        let c2 = Collider::aabb(100.0, 0.0, 100.0, 100.0);
        assert!(!check_collision(&c1, &c2)); // Kenarlar sadece dokunuyor
    }

    #[test]
    fn test_aabb_aabb_overlap() {
        let c1 = Collider::aabb(0.0, 0.0, 100.0, 100.0);
        let c2 = Collider::aabb(99.0, 99.0, 100.0, 100.0);
        assert!(check_collision(&c1, &c2));
    }

    #[test]
    fn test_circle_circle_collision_true() {
        let c1 = Collider::circle(0.0, 0.0, 50.0);
        let c2 = Collider::circle(75.0, 0.0, 50.0);
        assert!(check_collision(&c1, &c2));
    }

    #[test]
    fn test_circle_circle_no_collision() {
        let c1 = Collider::circle(0.0, 0.0, 50.0);
        let c2 = Collider::circle(200.0, 200.0, 50.0);
        assert!(!check_collision(&c1, &c2));
    }

    #[test]
    fn test_circle_circle_touching() {
        let c1 = Collider::circle(0.0, 0.0, 50.0);
        let c2 = Collider::circle(100.0, 0.0, 50.0);
        assert!(!check_collision(&c1, &c2)); // Tam teması değil
    }

    #[test]
    fn test_aabb_circle_collision_true() {
        let aabb = Collider::aabb(0.0, 0.0, 100.0, 100.0);
        let circle = Collider::circle(50.0, 50.0, 30.0); // Daire AABB içinde
        assert!(check_collision(&aabb, &circle));
    }

    #[test]
    fn test_aabb_circle_no_collision() {
        let aabb = Collider::aabb(0.0, 0.0, 100.0, 100.0);
        let circle = Collider::circle(200.0, 200.0, 30.0);
        assert!(!check_collision(&aabb, &circle));
    }

    #[test]
    fn test_aabb_circle_edge_collision() {
        let aabb = Collider::aabb(0.0, 0.0, 100.0, 100.0);
        let circle = Collider::circle(110.0, 50.0, 20.0);
        assert!(check_collision(&aabb, &circle));
    }

    #[test]
    fn test_circle_aabb_reverse_order() {
        let circle = Collider::circle(50.0, 50.0, 30.0);
        let aabb = Collider::aabb(0.0, 0.0, 100.0, 100.0);
        assert!(check_collision(&circle, &aabb));
    }

    #[test]
    fn test_is_colliding_alias() {
        let c1 = Collider::aabb(0.0, 0.0, 100.0, 100.0);
        let c2 = Collider::aabb(50.0, 50.0, 100.0, 100.0);
        assert!(is_colliding(&c1, &c2));
    }

    #[test]
    fn test_complex_collision_scenario() {
        // Üç collider'ı test et
        let obstacle = Collider::aabb(50.0, 50.0, 50.0, 50.0);
        let player = Collider::circle(30.0, 30.0, 20.0);
        let enemy = Collider::circle(100.0, 100.0, 25.0);

        assert!(!check_collision(&player, &obstacle));
        assert!(!check_collision(&player, &enemy));
        assert!(check_collision(&enemy, &obstacle));
    }
}
