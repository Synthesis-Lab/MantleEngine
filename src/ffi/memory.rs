/// Pinned memory management for zero-copy FFI
/// 
/// Rust's memory layout is unstable for C++, so we use:
/// - Arc<Mutex> to prevent freeing while C++ reads
/// - Box pinned to fixed address
/// - Manual lifetime tracking

use std::sync::Arc;
use std::sync::Mutex;

/// A pinned, thread-safe buffer for FFI
pub struct PinnedBuffer<T> {
    // Box ensures fixed address; Arc ensures shared ownership
    // Mutex prevents concurrent Rust/C++ writes
    data: Arc<Mutex<Box<T>>>,
}

impl<T> Clone for PinnedBuffer<T> {
    fn clone(&self) -> Self {
        PinnedBuffer {
            data: Arc::clone(&self.data),
        }
    }
}

impl<T> PinnedBuffer<T> {
    /// Create new pinned buffer
    pub fn new(value: T) -> Self {
        PinnedBuffer {
            data: Arc::new(Mutex::new(Box::new(value))),
        }
    }

    /// Get mutable access (locks mutex)
    pub fn lock(&self) -> std::sync::MutexGuard<'_, Box<T>> {
        self.data.lock().unwrap()
    }

    /// Get reference count (for debugging)
    pub fn strong_count(&self) -> usize {
        Arc::strong_count(&self.data)
    }
}

/// Arena allocator for render frame packets
/// 
/// Allocates all render data in a single contiguous block.
/// This minimizes allocations and improves cache locality.
///
/// Lifetime: One allocation per rendered frame
pub struct RenderArena {
    // Placeholder for now; will be expanded in Phase 3b
    capacity: usize,
}

impl RenderArena {
    /// Create a new arena with given capacity
    pub fn new(capacity: usize) -> Self {
        RenderArena { capacity }
    }

    /// Get arena capacity
    pub fn capacity(&self) -> usize {
        self.capacity
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_pinned_buffer_creation() {
        let buf = PinnedBuffer::new(42i32);
        let locked = buf.lock();
        assert_eq!(**locked, 42);
    }

    #[test]
    fn test_pinned_buffer_clone() {
        let buf = PinnedBuffer::new(vec![1, 2, 3]);
        let buf2 = buf.clone();
        
        assert_eq!(buf.strong_count(), 2);
        assert_eq!(buf2.strong_count(), 2);
    }

    #[test]
    fn test_render_arena_creation() {
        let arena = RenderArena::new(1024 * 1024);
        assert_eq!(arena.capacity(), 1024 * 1024);
    }
}
