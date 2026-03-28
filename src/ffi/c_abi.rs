/// C ABI Interface — Safe function definitions for C++ interoperability
/// 
/// All functions use C calling convention for compatibility with C++.
/// Error handling uses return codes since exceptions don't cross FFI boundaries.
///
/// Memory Safety:
/// - All pointers must come from valid RenderArena allocations
/// - Caller must ensure packets remain valid until render completes
/// - C++ side must NOT free Rust-allocated memory

use std::os::raw::{c_char, c_void};
use crate::ffi::{RendererHandle, RenderPacket};

/// Error codes for FFI operations
#[repr(C)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum FFIError {
    /// Operation succeeded
    OK = 0,
    /// Invalid pointer or handle
    InvalidPointer = 1,
    /// Invalid parameter
    InvalidParameter = 2,
    /// Renderer not initialized
    RendererNotInitialized = 3,
    /// Memory allocation failed
    AllocationFailed = 4,
    /// Internal error
    InternalError = 5,
}

/// Initialize the renderer
/// 
/// # Safety
/// - Must be called exactly once before any render operations
/// - Returns a RendererHandle to use in subsequent calls
/// - C++ caller must keep handle valid
#[unsafe(no_mangle)]
pub extern "C" fn mantle_renderer_init() -> RendererHandle {
    // Phase 4: Will create actual C++ renderer instance
    // For now, return valid handle
    RendererHandle(0x1 as *mut c_void) // Non-null placeholder
}

/// Shutdown the renderer and free resources
/// 
/// # Safety
/// - Must only be called with valid handle from mantle_renderer_init
/// - After calling, handle is invalid
/// - No render calls permitted after shutdown
#[unsafe(no_mangle)]
pub extern "C" fn mantle_renderer_shutdown(handle: RendererHandle) -> FFIError {
    if !handle.is_valid() {
        return FFIError::InvalidPointer;
    }
    
    // Phase 4: Will clean up C++ renderer instance
    FFIError::OK
}

/// Submit a render packet to the renderer
/// 
/// # Safety
/// - packet must point to valid RenderPacket
/// - All array pointers in packet must remain valid
/// - C++ will process asynchronously; don't free until next frame
///
/// # Returns
/// - FFIError::OK if accepted
/// - FFIError::InvalidPointer if handle or packet invalid
/// - FFIError::RendererNotInitialized if renderer not ready
#[unsafe(no_mangle)]
pub extern "C" fn mantle_render_submit(
    handle: RendererHandle,
    packet: *const RenderPacket,
) -> FFIError {
    if !handle.is_valid() {
        return FFIError::RendererNotInitialized;
    }
    
    if packet.is_null() {
        return FFIError::InvalidPointer;
    }
    
    // Safety: packet pointer is validated above
    let _packet = unsafe { &*packet };
    
    // Phase 4: Pass to C++ renderer queue
    // For now, just validate structure
    FFIError::OK
}

/// Get renderer status
/// 
/// # Safety
/// - handle must be valid
///
/// # Returns
/// - 1 if ready to render
/// - 0 if not ready
/// - -1 if invalid handle
#[unsafe(no_mangle)]
pub extern "C" fn mantle_renderer_is_ready(handle: RendererHandle) -> i32 {
    if handle.is_valid() {
        1 // Phase 4: Will check actual renderer status
    } else {
        -1
    }
}

/// Allocate memory from the render arena
/// 
/// # Safety
/// - Returns pointer to newly allocated memory
/// - Must be freed with mantle_arena_free (not free/delete)
/// - Allocation size must match original request
///
/// # Returns
/// - Non-null pointer on success
/// - Null pointer on allocation failure
#[unsafe(no_mangle)]
pub extern "C" fn mantle_arena_allocate(size: usize) -> *mut c_void {
    if size == 0 {
        return std::ptr::null_mut();
    }
    
    // Phase 3b: Simple allocation (Phase 3b-final will use actual arena)
    let layout = std::alloc::Layout::from_size_align(size, std::mem::align_of::<usize>())
        .expect("Invalid layout");
    
    unsafe {
        let ptr = std::alloc::alloc(layout);
        if ptr.is_null() {
            eprintln!("Arena allocation failed: {} bytes", size);
        }
        ptr as *mut c_void
    }
}

/// Free arena-allocated memory
/// 
/// # Safety
/// - ptr must come from mantle_arena_allocate
/// - size must exactly match original allocation
/// - Calling twice results in undefined behavior
///
/// # Returns
/// - FFIError::OK if successful
/// - FFIError::InvalidPointer if ptr is null or invalid
#[unsafe(no_mangle)]
pub extern "C" fn mantle_arena_free(ptr: *mut c_void, size: usize) -> FFIError {
    if ptr.is_null() {
        return FFIError::InvalidPointer;
    }
    
    if size == 0 {
        return FFIError::InvalidParameter;
    }
    
    // Safety: Caller must ensure ptr/size are valid
    let layout = std::alloc::Layout::from_size_align(size, std::mem::align_of::<usize>())
        .expect("Invalid layout");
    
    unsafe {
        std::alloc::dealloc(ptr as *mut u8, layout);
    }
    
    FFIError::OK
}

/// Get last error message
/// 
/// # Safety
/// - Returned pointer remains valid until next error
/// - Do NOT free the returned pointer
/// - Message is UTF-8 encoded
///
/// # Returns
/// - C string pointer to error message or empty string
#[unsafe(no_mangle)]
pub extern "C" fn mantle_get_last_error() -> *const c_char {
    // Phase 4: Implement actual error tracking
    // For now, return empty string
    b"\0".as_ptr() as *const c_char
}

/// Vertex/Index buffer upload (Phase 4 integration point)
/// 
/// # Safety
/// - data must be valid for 'size' bytes
/// - out_id must point to writable u32 for resource ID
#[unsafe(no_mangle)]
pub extern "C" fn mantle_upload_buffer(
    handle: RendererHandle,
    data: *const c_void,
    size: usize,
    out_id: *mut u32,
) -> FFIError {
    if !handle.is_valid() {
        return FFIError::RendererNotInitialized;
    }
    
    if data.is_null() || size == 0 {
        return FFIError::InvalidPointer;
    }
    
    if out_id.is_null() {
        return FFIError::InvalidPointer;
    }
    
    // Safety: pointers are checked above
    unsafe {
        *out_id = 0; // Phase 4: Will assign actual resource ID
    }
    
    FFIError::OK
}

/// Wait for render to complete
/// 
/// # Safety
/// - Can be called safely from any thread
///
/// # Returns
/// - FFIError::OK if render completed
/// - FFIError::RendererNotInitialized if not ready
#[unsafe(no_mangle)]
pub extern "C" fn mantle_wait_render(handle: RendererHandle) -> FFIError {
    if !handle.is_valid() {
        return FFIError::RendererNotInitialized;
    }
    
    // Phase 4: Will synchronize with C++ render thread
    FFIError::OK
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::ffi::CStr;

    #[test]
    fn test_ffi_error_codes() {
        assert_eq!(FFIError::OK as i32, 0);
        assert_eq!(FFIError::InvalidPointer as i32, 1);
        assert_eq!(FFIError::InvalidParameter as i32, 2);
    }

    #[test]
    fn test_renderer_init_shutdown() {
        let handle = mantle_renderer_init();
        assert!(handle.is_valid());
        
        let result = mantle_renderer_shutdown(handle);
        assert_eq!(result, FFIError::OK);
    }

    #[test]
    fn test_renderer_shutdown_invalid_handle() {
        let null_handle = RendererHandle::null();
        let result = mantle_renderer_shutdown(null_handle);
        assert_eq!(result, FFIError::InvalidPointer);
    }

    #[test]
    fn test_renderer_is_ready() {
        let handle = mantle_renderer_init();
        assert_eq!(mantle_renderer_is_ready(handle), 1);
        
        let null_handle = RendererHandle::null();
        assert_eq!(mantle_renderer_is_ready(null_handle), -1);
    }

    #[test]
    fn test_arena_allocate_free() {
        let ptr = mantle_arena_allocate(256);
        assert!(!ptr.is_null());
        
        let result = mantle_arena_free(ptr, 256);
        assert_eq!(result, FFIError::OK);
    }

    #[test]
    fn test_arena_allocate_zero() {
        let ptr = mantle_arena_allocate(0);
        assert!(ptr.is_null());
    }

    #[test]
    fn test_arena_free_null() {
        let result = mantle_arena_free(std::ptr::null_mut(), 256);
        assert_eq!(result, FFIError::InvalidPointer);
    }

    #[test]
    fn test_arena_free_zero_size() {
        let ptr = mantle_arena_allocate(1);
        assert!(!ptr.is_null());
        
        let result = mantle_arena_free(ptr, 0);
        assert_eq!(result, FFIError::InvalidParameter);
    }

    #[test]
    fn test_render_submit_null_packet() {
        let handle = mantle_renderer_init();
        let result = mantle_render_submit(handle, std::ptr::null());
        assert_eq!(result, FFIError::InvalidPointer);
    }

    #[test]
    fn test_render_submit_invalid_handle() {
        let null_handle = RendererHandle::null();
        let packet = RenderPacket::new(0, 0.016);
        let result = mantle_render_submit(null_handle, &packet);
        assert_eq!(result, FFIError::RendererNotInitialized);
    }

    #[test]
    fn test_upload_buffer_null_handle() {
        let null_handle = RendererHandle::null();
        let data = [0u8; 16];
        let mut id = 0u32;
        let result = mantle_upload_buffer(null_handle, data.as_ptr() as *const c_void, 16, &mut id);
        assert_eq!(result, FFIError::RendererNotInitialized);
    }

    #[test]
    fn test_upload_buffer_null_data() {
        let handle = mantle_renderer_init();
        let mut id = 0u32;
        let result = mantle_upload_buffer(handle, std::ptr::null(), 16, &mut id);
        assert_eq!(result, FFIError::InvalidPointer);
    }

    #[test]
    fn test_wait_render_invalid_handle() {
        let null_handle = RendererHandle::null();
        let result = mantle_wait_render(null_handle);
        assert_eq!(result, FFIError::RendererNotInitialized);
    }

    #[test]
    fn test_get_last_error() {
        let error_ptr = mantle_get_last_error();
        assert!(!error_ptr.is_null());
        
        // Should be valid C string (at least null terminator)
        let _cstr = unsafe { CStr::from_ptr(error_ptr) };
    }
}
