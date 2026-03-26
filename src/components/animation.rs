use glam::Vec2;
use bevy_ecs::component::Component;

/// Bir animasyon frame'i: UV koordinatları ve süresi
#[derive(Clone, Debug, PartialEq, Component)]
pub struct AnimationFrame {
    /// Frame'in başlangıç UV koordinatı
    pub uv_min: Vec2,
    /// Frame'in bitiş UV koordinatı
    pub uv_max: Vec2,
    /// Frame'in gösterilme süresi (saniye)
    pub duration: f32,
}

impl AnimationFrame {
    /// Yeni bir AnimationFrame oluştur
    pub fn new(uv_min: Vec2, uv_max: Vec2, duration: f32) -> Self {
        AnimationFrame {
            uv_min,
            uv_max,
            duration,
        }
    }

    /// UV dikdörtgeni ile frame oluştur (x, y, width, height)
    pub fn from_uv_rect(x: f32, y: f32, w: f32, h: f32, duration: f32) -> Self {
        AnimationFrame {
            uv_min: Vec2::new(x, y),
            uv_max: Vec2::new(x + w, y + h),
            duration,
        }
    }
}

/// Bir tam animasyon: frame'lerin sırası, hız, looping
#[derive(Clone, Debug, Component)]
pub struct Animation {
    /// Animasyonun adı
    pub name: String,
    /// Frame'lerin listesi
    pub frames: Vec<AnimationFrame>,
    /// Çalma hızı çarpanı (1.0 = normal, 2.0 = iki kat hızlı)
    pub speed: f32,
    /// Animasyon sonunda başa dön mi?
    pub looping: bool,
}

impl Animation {
    /// Yeni bir Animation oluştur
    pub fn new(name: String, speed: f32, looping: bool) -> Self {
        Animation {
            name,
            frames: Vec::new(),
            speed,
            looping,
        }
    }

    /// Frame ekle
    pub fn add_frame(&mut self, frame: AnimationFrame) {
        self.frames.push(frame);
    }

    /// Frame'leri toplamla ekle
    pub fn add_frames(&mut self, frames: Vec<AnimationFrame>) {
        self.frames.extend(frames);
    }

    /// Toplam süre (bütün frame'lerin önceden hızlanmış)
    pub fn total_duration(&self) -> f32 {
        self.frames.iter().map(|f| f.duration).sum::<f32>() / self.speed
    }

    /// Frame sayısı
    pub fn frame_count(&self) -> usize {
        self.frames.len()
    }

    /// İlk frame'i al
    pub fn first_frame(&self) -> Option<&AnimationFrame> {
        self.frames.first()
    }

    /// Son frame'i al
    pub fn last_frame(&self) -> Option<&AnimationFrame> {
        self.frames.last()
    }

    /// Belirtilen index'teki frame'i al
    pub fn get_frame(&self, index: usize) -> Option<&AnimationFrame> {
        if index < self.frames.len() {
            Some(&self.frames[index])
        } else {
            None
        }
    }

    /// String gösterimi
    pub fn to_string(&self) -> String {
        format!(
            "Animation(name={}, frames={}, speed={}, looping={})",
            self.name,
            self.frames.len(),
            self.speed,
            self.looping
        )
    }
}

/// Animasyon oynatma durumu
#[derive(Clone, Debug, Component)]
pub struct AnimationState {
    /// Mevcut frame'in index'i
    pub frame_index: usize,
    /// Bu frame'de geçmiş süre (saniye)
    pub elapsed_time: f32,
    /// Oynatılıyor mu?
    pub is_playing: bool,
}

impl AnimationState {
    /// Yeni bir AnimationState oluştur (başlangıçta stopped)
    pub fn new() -> Self {
        AnimationState {
            frame_index: 0,
            elapsed_time: 0.0,
            is_playing: false,
        }
    }

    /// Başla
    pub fn play(&mut self) {
        self.is_playing = true;
    }

    /// Duraklat
    pub fn pause(&mut self) {
        self.is_playing = false;
    }

    /// Durdur ve başlangıca dön
    pub fn stop(&mut self) {
        self.is_playing = false;
        self.frame_index = 0;
        self.elapsed_time = 0.0;
    }

    /// Durumu güncelle (delta_time: bu frame'de geçen süre saniye cinsinden)
    pub fn update(&mut self, delta_time: f32, animation: &Animation) {
        if !self.is_playing || animation.frames.is_empty() {
            return;
        }

        // Hızlandırılmış delta_time
        let scaled_delta = delta_time * animation.speed;
        self.elapsed_time += scaled_delta;

        // Mevcut frame'in süresini kontrol et
        if let Some(current_frame) = animation.get_frame(self.frame_index) {
            if self.elapsed_time >= current_frame.duration {
                // Sonraki frame'e geç
                self.elapsed_time -= current_frame.duration;
                self.frame_index += 1;

                // Son frame'den sonra?
                if self.frame_index >= animation.frames.len() {
                    if animation.looping {
                        self.frame_index = 0;
                    } else {
                        self.frame_index = animation.frames.len() - 1;
                        self.is_playing = false;
                    }
                }
            }
        }
    }

    /// Mevcut UV koordinatlarını al
    pub fn current_uv(&self, animation: &Animation) -> Option<(glam::Vec2, glam::Vec2)> {
        animation
            .get_frame(self.frame_index)
            .map(|f| (f.uv_min, f.uv_max))
    }

    /// String gösterimi
    pub fn to_string(&self) -> String {
        format!(
            "AnimationState(frame={}, time={:.2}, playing={})",
            self.frame_index, self.elapsed_time, self.is_playing
        )
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_animation_frame_creation() {
        let frame = AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.5, 0.5), 0.1);
        assert_eq!(frame.uv_min, Vec2::new(0.0, 0.0));
        assert_eq!(frame.uv_max, Vec2::new(0.5, 0.5));
        assert_eq!(frame.duration, 0.1);
    }

    #[test]
    fn test_animation_frame_from_uv_rect() {
        let frame = AnimationFrame::from_uv_rect(0.0, 0.0, 0.25, 0.25, 0.1);
        assert_eq!(frame.uv_min, Vec2::new(0.0, 0.0));
        assert_eq!(frame.uv_max, Vec2::new(0.25, 0.25));
    }

    #[test]
    fn test_animation_creation() {
        let anim = Animation::new("idle".to_string(), 1.0, true);
        assert_eq!(anim.name, "idle");
        assert_eq!(anim.speed, 1.0);
        assert!(anim.looping);
        assert_eq!(anim.frame_count(), 0);
    }

    #[test]
    fn test_animation_add_frame() {
        let mut anim = Animation::new("walk".to_string(), 1.0, false);
        let frame1 = AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.25, 1.0), 0.1);
        let frame2 = AnimationFrame::new(Vec2::new(0.25, 0.0), Vec2::new(0.5, 1.0), 0.1);

        anim.add_frame(frame1);
        anim.add_frame(frame2);

        assert_eq!(anim.frame_count(), 2);
    }

    #[test]
    fn test_animation_total_duration() {
        let mut anim = Animation::new("run".to_string(), 2.0, false);
        anim.add_frame(AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.25, 1.0), 0.1));
        anim.add_frame(AnimationFrame::new(Vec2::new(0.25, 0.0), Vec2::new(0.5, 1.0), 0.1));
        anim.add_frame(AnimationFrame::new(Vec2::new(0.5, 0.0), Vec2::new(0.75, 1.0), 0.1));

        // Total duration = 0.3 / 2.0 = 0.15
        assert!((anim.total_duration() - 0.15).abs() < 0.001);
    }

    #[test]
    fn test_animation_get_frame() {
        let mut anim = Animation::new("test".to_string(), 1.0, false);
        let frame1 = AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.5, 0.5), 0.1);
        let frame2 = AnimationFrame::new(Vec2::new(0.5, 0.0), Vec2::new(1.0, 0.5), 0.1);

        anim.add_frame(frame1.clone());
        anim.add_frame(frame2.clone());

        assert_eq!(anim.get_frame(0), Some(&frame1));
        assert_eq!(anim.get_frame(1), Some(&frame2));
        assert_eq!(anim.get_frame(2), None);
    }

    #[test]
    fn test_animation_state_creation() {
        let state = AnimationState::new();
        assert_eq!(state.frame_index, 0);
        assert_eq!(state.elapsed_time, 0.0);
        assert!(!state.is_playing);
    }

    #[test]
    fn test_animation_state_play_pause_stop() {
        let mut state = AnimationState::new();

        state.play();
        assert!(state.is_playing);

        state.pause();
        assert!(!state.is_playing);

        state.play();
        state.elapsed_time = 5.0;
        state.frame_index = 3;

        state.stop();
        assert!(!state.is_playing);
        assert_eq!(state.frame_index, 0);
        assert_eq!(state.elapsed_time, 0.0);
    }

    #[test]
    fn test_animation_state_update_looping() {
        let mut anim = Animation::new("loop_test".to_string(), 1.0, true);
        anim.add_frame(AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.5, 0.5), 0.1));
        anim.add_frame(AnimationFrame::new(Vec2::new(0.5, 0.0), Vec2::new(1.0, 0.5), 0.1));

        let mut state = AnimationState::new();
        state.play();

        // İlk frame'i geç
        state.update(0.1, &anim);
        assert_eq!(state.frame_index, 1);

        // İkinci frame'i geç, looping olmalı
        state.update(0.1, &anim);
        assert_eq!(state.frame_index, 0);
        assert!(state.is_playing); // Hala oynatılıyor
    }

    #[test]
    fn test_animation_state_update_no_looping() {
        let mut anim = Animation::new("no_loop".to_string(), 1.0, false);
        anim.add_frame(AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.5, 0.5), 0.1));
        anim.add_frame(AnimationFrame::new(Vec2::new(0.5, 0.0), Vec2::new(1.0, 0.5), 0.1));

        let mut state = AnimationState::new();
        state.play();

        // İlk frame'i geç
        state.update(0.1, &anim);
        assert_eq!(state.frame_index, 1);

        // İkinci frame'i geç, sona ulaşmalı
        state.update(0.1, &anim);
        assert_eq!(state.frame_index, 1);
        assert!(!state.is_playing); // Durmuş
    }

    #[test]
    fn test_animation_state_speed_multiplier() {
        let mut anim = Animation::new("fast".to_string(), 2.0, false);
        anim.add_frame(AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.5, 0.5), 0.1));
        anim.add_frame(AnimationFrame::new(Vec2::new(0.5, 0.0), Vec2::new(1.0, 0.5), 0.1));

        let mut state = AnimationState::new();
        state.play();

        // 0.05 saniye geç (2x hız = 0.1 saniye frame süresi = frame değişir)
        state.update(0.05, &anim);
        assert_eq!(state.frame_index, 1);
    }

    #[test]
    fn test_animation_state_current_uv() {
        let mut anim = Animation::new("uv_test".to_string(), 1.0, false);
        let frame1 = AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.5, 0.5), 0.1);
        let frame2 = AnimationFrame::new(Vec2::new(0.5, 0.0), Vec2::new(1.0, 0.5), 0.1);

        anim.add_frame(frame1.clone());
        anim.add_frame(frame2.clone());

        let state = AnimationState::new();
        let (uv_min, uv_max) = state.current_uv(&anim).unwrap();
        assert_eq!(uv_min, Vec2::new(0.0, 0.0));
        assert_eq!(uv_max, Vec2::new(0.5, 0.5));
    }

    #[test]
    fn test_animation_state_not_playing() {
        let mut anim = Animation::new("test".to_string(), 1.0, false);
        anim.add_frame(AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.5, 0.5), 0.1));

        let mut state = AnimationState::new();
        let initial_frame = state.frame_index;

        state.update(0.2, &anim); // Oynatılmıyor panicked

        assert_eq!(state.frame_index, initial_frame); // Değişmemiş
    }

    #[test]
    fn test_animation_frame_equality() {
        let f1 = AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.5, 0.5), 0.1);
        let f2 = AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.5, 0.5), 0.1);
        assert_eq!(f1, f2);
    }

    #[test]
    fn test_animation_state_to_string() {
        let mut state = AnimationState::new();
        state.frame_index = 2;
        state.elapsed_time = 0.5;
        state.is_playing = true;

        let s = state.to_string();
        assert!(s.contains("frame=2"));
        assert!(s.contains("playing=true"));
    }

    #[test]
    fn test_animation_to_string() {
        let mut anim = Animation::new("walk".to_string(), 1.5, true);
        anim.add_frame(AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.25, 1.0), 0.1));
        anim.add_frame(AnimationFrame::new(Vec2::new(0.25, 0.0), Vec2::new(0.5, 1.0), 0.1));

        let s = anim.to_string();
        assert!(s.contains("walk"));
        assert!(s.contains("frames=2"));
    }

    #[test]
    fn test_animation_first_last_frame() {
        let mut anim = Animation::new("test".to_string(), 1.0, false);
        let f1 = AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.25, 1.0), 0.1);
        let f2 = AnimationFrame::new(Vec2::new(0.25, 0.0), Vec2::new(0.5, 1.0), 0.1);
        let f3 = AnimationFrame::new(Vec2::new(0.5, 0.0), Vec2::new(0.75, 1.0), 0.1);

        anim.add_frame(f1.clone());
        anim.add_frame(f2.clone());
        anim.add_frame(f3.clone());

        assert_eq!(anim.first_frame(), Some(&f1));
        assert_eq!(anim.last_frame(), Some(&f3));
    }

    #[test]
    fn test_animation_add_frames_bulk() {
        let mut anim = Animation::new("bulk".to_string(), 1.0, false);
        let frames = vec![
            AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.25, 1.0), 0.1),
            AnimationFrame::new(Vec2::new(0.25, 0.0), Vec2::new(0.5, 1.0), 0.1),
            AnimationFrame::new(Vec2::new(0.5, 0.0), Vec2::new(0.75, 1.0), 0.1),
        ];

        anim.add_frames(frames);
        assert_eq!(anim.frame_count(), 3);
    }

    #[test]
    fn test_animation_state_multiple_updates() {
        let mut anim = Animation::new("multi".to_string(), 1.0, true);
        anim.add_frame(AnimationFrame::new(Vec2::new(0.0, 0.0), Vec2::new(0.33, 1.0), 0.1));
        anim.add_frame(AnimationFrame::new(Vec2::new(0.33, 0.0), Vec2::new(0.66, 1.0), 0.1));
        anim.add_frame(AnimationFrame::new(Vec2::new(0.66, 0.0), Vec2::new(1.0, 1.0), 0.1));

        let mut state = AnimationState::new();
        state.play();

        state.update(0.1, &anim); // Frame 1
        assert_eq!(state.frame_index, 1);

        state.update(0.1, &anim); // Frame 2
        assert_eq!(state.frame_index, 2);

        state.update(0.1, &anim); // Frame 0 (looping)
        assert_eq!(state.frame_index, 0);
    }
}
