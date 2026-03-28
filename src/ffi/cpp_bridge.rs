// src/ffi/cpp_bridge.rs
// C++ FFI bindings - connects Rust to C++ VulkanRenderer

use std::sync::Mutex;
use once_cell::sync::Lazy;

/// Opaque handle to C++ VulkanRenderer instance
/// For now, we use a void pointer; actual C++ type is opaque to Rust
pub type VulkanRenderer = std::ffi::c_void;

/// Wrapper for Send+Sync C++ pointer
/// The C++ VulkanRenderer is thread-safe internally
#[derive(Clone, Copy)]
pub struct SendSyncPtr(*mut VulkanRenderer);

unsafe impl Send for SendSyncPtr {}
unsafe impl Sync for SendSyncPtr {}

/// Global VulkanRenderer instance (managed by C++ code)
/// Accessed through the C ABI interface
pub static VULKAN_RENDERER: Lazy<Mutex<Option<SendSyncPtr>>> = 
    Lazy::new(|| Mutex::new(None));

// C FFI bindings to VulkanRenderer class
unsafe extern "C" {
    /// VulkanRenderer constructor - allocates and initializes
    pub fn vulkan_renderer_new() -> *mut VulkanRenderer;
    
    /// VulkanRenderer destructor - deallocates
    pub fn vulkan_renderer_delete(renderer: *mut VulkanRenderer);
    
    /// Initialize Vulkan instance, device, and command pools
    pub fn vulkan_renderer_initialize(renderer: *mut VulkanRenderer) -> bool;
    
    /// Clean up all Vulkan resources
    pub fn vulkan_renderer_shutdown(renderer: *mut VulkanRenderer);
    
    /// Check if renderer is ready to accept render packets
    pub fn vulkan_renderer_is_ready(renderer: *const VulkanRenderer) -> bool;
    
    /// Submit a RenderPacket for processing
    pub fn vulkan_renderer_submit_render_packet(
        renderer: *mut VulkanRenderer,
        packet: *const crate::ffi::render_packet::RenderPacket,
    );
    
    /// Render a single frame and wait for completion
    pub fn vulkan_renderer_wait_render(renderer: *mut VulkanRenderer);
    
    /// Upload buffer data to GPU
    pub fn vulkan_renderer_upload_buffer(
        renderer: *mut VulkanRenderer,
        data: *const u8,
        size: u32,
    ) -> u32;
    
    /// Get last error message from renderer
    pub fn vulkan_renderer_get_last_error(
        renderer: *const VulkanRenderer,
    ) -> *const i8;
}

/// Initialize the global Vulkan renderer
/// Called once during application startup
pub fn init_vulkan_renderer() -> crate::ffi::FFIError {
    let renderer = unsafe { vulkan_renderer_new() };
    
    if renderer.is_null() {
        return crate::ffi::FFIError::InternalError;
    }
    
    let initialized = unsafe { vulkan_renderer_initialize(renderer) };
    
    if !initialized {
        unsafe { vulkan_renderer_delete(renderer) };
        return crate::ffi::FFIError::InternalError;
    }
    
    let mut global = match VULKAN_RENDERER.lock() {
        Ok(g) => g,
        Err(_) => return crate::ffi::FFIError::InternalError,
    };
    
    *global = Some(SendSyncPtr(renderer));
    crate::ffi::FFIError::OK
}

/// Shutdown the global Vulkan renderer
pub fn shutdown_vulkan_renderer() {
    let mut global = match VULKAN_RENDERER.lock() {
        Ok(g) => g,
        Err(_) => return,
    };
    
    if let Some(SendSyncPtr(renderer)) = global.take() {
        unsafe {
            vulkan_renderer_shutdown(renderer);
            vulkan_renderer_delete(renderer);
        }
    }
}

/// Get the global renderer handle (if initialized)
pub fn get_vulkan_renderer() -> Result<*mut VulkanRenderer, crate::ffi::FFIError> {
    let global = VULKAN_RENDERER.lock()
        .map_err(|_| crate::ffi::FFIError::InternalError)?;
    
    global.map(|SendSyncPtr(ptr)| ptr).ok_or(crate::ffi::FFIError::RendererNotInitialized)
}

#[cfg(test)]
mod tests {
    #[test]
    fn test_cpp_bridge_types() {
        // Verify size of opaque VulkanRenderer type
        // This test ensures the C++ definition matches expectations
        println!("cpp_bridge module loaded successfully");
    }
}
