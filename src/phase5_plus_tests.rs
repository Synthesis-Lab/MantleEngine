#[cfg(test)]
mod phase_5_plus_integration_tests {
    use crate::*;

    #[test]
    fn test_game_loop_orchestration() {
        let mut core = MantleCore::new();
        
        // Run first frame
        let stats = core.update_frame(0.016); // 60 FPS
        assert_eq!(stats.frame_number, 0);
        assert_eq!(stats.delta_time, 0.016);
        assert_eq!(core.frame_number(), 1);
        
        // Run second frame
        let stats = core.update_frame(0.016);
        assert_eq!(stats.frame_number, 1);
        assert_eq!(core.frame_number(), 2);
    }

    #[test]
    fn test_render_data_addition() {
        let mut core = MantleCore::new();
        
        core.add_render_transforms(vec![(1, 10.0, 20.0), (2, 30.0, 40.0)]);
        core.add_render_sprites(vec![(1, "player.png".to_string())]);
        core.add_render_colliders(vec![( 3, 5.0, 10.0)]);
        
        let packet = core.build_render_packet();
        assert_eq!(packet.transform_count, 2);
        assert_eq!(packet.sprite_count, 1);
        assert_eq!(packet.collider_count, 1);
    }

    #[test]
    fn test_collision_queries() {
        let mut core = MantleCore::new();
        
        // Report some collisions
        core.collision_system.report_collision(1, 2, 10.0, 20.0);
        core.collision_system.report_collision(3, 4, 30.0, 40.0);
        
        assert_eq!(core.active_collisions().len(), 2);
        assert!(core.are_colliding(1, 2));
        assert!(!core.are_colliding(1, 5));
        
        // Move to next frame
        core.collision_system.next_frame();
        core.collision_system.report_collision(1, 2, 11.0, 21.0); // Still colliding
        
        let new_collisions = core.new_collisions();
        let ended_collisions = core.ended_collisions();
        
        // 3,4 ended, 1,2 continues (not in new)
        assert_eq!(ended_collisions.len(), 1);
        assert_eq!(new_collisions.len(), 0);
    }

    #[test]
    fn test_animation_system_integration() {
        let mut core = MantleCore::new();
        
        // Register animation
        core.animation_system.register(1, 10, 0.1); // 10 frames, 0.1s per frame
        
        // Update one frame
        core.animation_system.update(0.05);
        
        let track = core.animation_system.get(1).unwrap();
        assert!(track.current_frame == 0 || track.current_frame == 1);
    }

    #[test]
    fn test_stats_collection() {
        let mut core = MantleCore::new();
        
        core.add_render_transforms(vec![(1, 0.0, 0.0)]);
        
        let _stats = core.update_frame(0.016);
        
        // All stat methods should be callable
        let _render_stats = core.render_stats();
        let _animation_stats = core.animation_stats();
        let _collision_stats = core.collision_stats();
    }
}
