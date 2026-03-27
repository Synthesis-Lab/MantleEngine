//! AnimationSystem - Phase 5 Animation State Management  
//!
//! Responsible for:
//! 1. Tracking animation playback state
//! 2. Frame progression and timing
//! 3. Event signaling on animation completion

use std::collections::HashMap;

/// Animation playback state
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum PlaybackState {
    Playing,
    Paused,
    Stopped,
}

/// Animation track - represents one animation instance
#[derive(Clone)]
pub struct AnimationTrack {
    pub id: u64,
    pub state: PlaybackState,
    pub current_frame: u32,
    pub total_frames: u32,
    pub frame_duration: f32,
    pub elapsed: f32,
    pub speed_multiplier: f32,
    pub looping: bool,
}

impl AnimationTrack {
    /// Create new animation track
    pub fn new(id: u64, total_frames: u32, frame_duration: f32) -> Self {
        Self {
            id,
            state: PlaybackState::Stopped,
            current_frame: 0,
            total_frames,
            frame_duration,
            elapsed: 0.0,
            speed_multiplier: 1.0,
            looping: false,
        }
    }

    /// Update animation state with delta time
    pub fn update(&mut self, delta_time: f32) {
        if self.state != PlaybackState::Playing {
            return;
        }

        self.elapsed += delta_time * self.speed_multiplier;

        while self.elapsed >= self.frame_duration && self.current_frame < self.total_frames - 1 {
            self.elapsed -= self.frame_duration;
            self.current_frame += 1;
        }

        // Handle end of animation
        if self.current_frame >= self.total_frames - 1 && self.elapsed >= self.frame_duration {
            if self.looping {
                self.current_frame = 0;
                self.elapsed = 0.0;
            } else {
                self.state = PlaybackState::Stopped;
            }
        }
    }

    /// Play animation
    pub fn play(&mut self) {
        self.state = PlaybackState::Playing;
    }

    /// Pause animation
    pub fn pause(&mut self) {
        self.state = PlaybackState::Paused;
    }

    /// Stop animation (resets to frame 0)
    pub fn stop(&mut self) {
        self.state = PlaybackState::Stopped;
        self.current_frame = 0;
        self.elapsed = 0.0;
    }

    /// Set playback speed
    pub fn set_speed(&mut self, multiplier: f32) {
        self.speed_multiplier = multiplier.max(0.0);
    }

    /// Set looping behavior
    pub fn set_looping(&mut self, looping: bool) {
        self.looping = looping;
    }
}

/// AnimationSystem - Manages multiple animation tracks
pub struct AnimationSystem {
    tracks: HashMap<u64, AnimationTrack>,
    update_count: u64,
}

impl AnimationSystem {
    /// Create new animation system
    pub fn new() -> Self {
        Self {
            tracks: HashMap::new(),
            update_count: 0,
        }
    }

    /// Register new animation track
    pub fn register(&mut self, id: u64, total_frames: u32, frame_duration: f32) {
        self.tracks
            .insert(id, AnimationTrack::new(id, total_frames, frame_duration));
    }

    /// Unregister animation track
    pub fn unregister(&mut self, id: u64) -> Option<AnimationTrack> {
        self.tracks.remove(&id)
    }

    /// Get animation track (read-only)
    pub fn get(&self, id: u64) -> Option<&AnimationTrack> {
        self.tracks.get(&id)
    }

    /// Get animation track (mutable)
    pub fn get_mut(&mut self, id: u64) -> Option<&mut AnimationTrack> {
        self.tracks.get_mut(&id)
    }

    /// Update all active animation tracks
    pub fn update(&mut self, delta_time: f32) {
        for track in self.tracks.values_mut() {
            track.update(delta_time);
        }
        self.update_count += 1;
    }

    /// Get active animation count
    pub fn active_count(&self) -> usize {
        self.tracks.iter().filter(|(_, t)| t.state == PlaybackState::Playing).count()
    }

    /// Get total track count
    pub fn track_count(&self) -> usize {
        self.tracks.len()
    }

    /// Get statistics
    pub fn stats(&self) -> AnimationStats {
        AnimationStats {
            track_count: self.tracks.len(),
            active_count: self.active_count(),
            total_updates: self.update_count,
        }
    }

    /// Reset system
    pub fn reset(&mut self) {
        self.tracks.clear();
        self.update_count = 0;
    }
}

impl Default for AnimationSystem {
    fn default() -> Self {
        Self::new()
    }
}

/// Statistics about animation processing
#[derive(Debug, Clone, Default)]
pub struct AnimationStats {
    pub track_count: usize,
    pub active_count: usize,
    pub total_updates: u64,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_animation_track_creation() {
        let track = AnimationTrack::new(1, 4, 0.1);
        assert_eq!(track.id, 1);
        assert_eq!(track.total_frames, 4);
        assert_eq!(track.state, PlaybackState::Stopped);
    }

    #[test]
    fn test_animation_track_playback() {
        let mut track = AnimationTrack::new(1, 4, 0.1);
        assert_eq!(track.state, PlaybackState::Stopped);

        track.play();
        assert_eq!(track.state, PlaybackState::Playing);

        track.pause();
        assert_eq!(track.state, PlaybackState::Paused);

        track.play();
        track.stop();
        assert_eq!(track.state, PlaybackState::Stopped);
        assert_eq!(track.current_frame, 0);
    }

    #[test]
    fn test_animation_track_frame_progression() {
        let mut track = AnimationTrack::new(1, 4, 0.1);
        track.play();

        // Update with frame duration
        track.update(0.1);
        assert_eq!(track.current_frame, 1);

        track.update(0.1);
        assert_eq!(track.current_frame, 2);

        track.update(0.1);
        assert_eq!(track.current_frame, 3);
    }

    #[test]
    fn test_animation_track_speed() {
        let mut track = AnimationTrack::new(1, 4, 0.1);
        track.set_speed(2.0);
        track.play();

        // Double speed = progresses twice as fast
        // elapsed = 0 + 0.1 * 2.0 = 0.2, which is >= 0.1 (frame_duration), so frame advances
        track.update(0.1);
        assert_eq!(track.current_frame, 1); // 0.2 >= 0.1, so frame advances to 1
        
        // elapsed = 0.1 (remaining) + 0.05*2 = 0.2 >= 0.1, so frame advances again
        track.update(0.05);
        assert_eq!(track.current_frame, 2); // elapsed = 0.2, frame advances to 2
    }

    #[test]
    fn test_animation_track_looping() {
        let mut track = AnimationTrack::new(1, 2, 0.1);
        track.set_looping(true);
        track.play();

        // Advance to end
        track.update(0.1);
        assert_eq!(track.current_frame, 1);

        // Advance past end - should wrap
        track.update(0.1);
        assert_eq!(track.current_frame, 0);
        assert_eq!(track.state, PlaybackState::Playing);
    }

    #[test]
    fn test_animation_system_register() {
        let mut system = AnimationSystem::new();
        system.register(1, 4, 0.1);

        assert_eq!(system.track_count(), 1);
        assert!(system.get(1).is_some());
    }

    #[test]
    fn test_animation_system_update() {
        let mut system = AnimationSystem::new();
        system.register(1, 4, 0.1);

        if let Some(track) = system.get_mut(1) {
            track.play();
        }

        system.update(0.1);
        assert_eq!(system.update_count, 1);
        assert_eq!(system.active_count(), 1);
    }

    #[test]
    fn test_animation_system_stats() {
        let mut system = AnimationSystem::new();
        system.register(1, 4, 0.1);
        system.register(2, 4, 0.1);

        if let Some(track) = system.get_mut(1) {
            track.play();
        }

        let stats = system.stats();
        assert_eq!(stats.track_count, 2);
        assert_eq!(stats.active_count, 1);
    }
}

