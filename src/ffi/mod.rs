/// FFI Bridge Module — Rust-C++ interoperability
/// 
/// Provides zero-copy data structures and C ABI functions
/// for communicating with the C++ Vulkan renderer.
/// 
/// Design:
/// - Single-threaded render thread (C++ side)
/// - RenderPacket holds all frame data
/// - Pinned memory prevents Rust GC moves
/// - Minimal allocation per frame

pub mod c_abi;
pub mod memory;
pub mod render_packet;
pub mod cpp_bridge;

pub use c_abi::{FFIError, mantle_renderer_init, mantle_renderer_shutdown, mantle_render_submit};
pub use render_packet::RenderPacket;
pub use memory::{RenderArena, PinnedBuffer};
pub use cpp_bridge::init_vulkan_renderer;

use std::ffi::c_void;

/// Opaque handle to the C++ renderer instance
#[repr(transparent)]
pub struct RendererHandle(*mut c_void);

impl RendererHandle {
    /// Create a null handle (renderer not initialized)
    pub fn null() -> Self {
        RendererHandle(std::ptr::null_mut())
    }

    /// Check if handle is valid
    pub fn is_valid(&self) -> bool {
        !self.0.is_null()
    }

    /// Get raw pointer for FFI calls
    pub fn as_ptr(&self) -> *mut c_void {
        self.0
    }
}

impl Clone for RendererHandle {
    fn clone(&self) -> Self {
        RendererHandle(self.0)
    }
}

impl Copy for RendererHandle {}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_renderer_handle_null() {
        let handle = RendererHandle::null();
        assert!(!handle.is_valid());
    }

    #[test]
    fn test_renderer_handle_copy() {
        let h1 = RendererHandle(0xDEADBEEF as *mut c_void);
        let h2 = h1;
        assert_eq!(h1.as_ptr() as usize, h2.as_ptr() as usize);
    }
}
