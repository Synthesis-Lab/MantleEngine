use std::time::SystemTime;

/// Log seviyesi
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum LogLevel {
    Debug = 0,
    Info = 1,
    Warn = 2,
    Error = 3,
}

impl std::fmt::Display for LogLevel {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            LogLevel::Debug => write!(f, "DEBUG"),
            LogLevel::Info => write!(f, "INFO "),
            LogLevel::Warn => write!(f, "WARN "),
            LogLevel::Error => write!(f, "ERROR"),
        }
    }
}

/// Mantle Logger — Basit timestamp ve level bilgili logging
pub struct MantleLogger {
    min_level: LogLevel,
}

impl MantleLogger {
    pub fn new(min_level: LogLevel) -> Self {
        Self { min_level }
    }

    pub fn set_level(&mut self, level: LogLevel) {
        self.min_level = level;
    }

    fn get_timestamp() -> String {
        use std::time::UNIX_EPOCH;
        
        match SystemTime::now().duration_since(UNIX_EPOCH) {
            Ok(duration) => {
                let secs = duration.as_secs();
                let millis = duration.subsec_millis();
                // Basit format: [HH:MM:SS.mmm]
                let hours = (secs % 86400) / 3600;
                let minutes = (secs % 3600) / 60;
                let seconds = secs % 60;
                format!("{:02}:{:02}:{:02}.{:03}", hours, minutes, seconds, millis)
            }
            Err(_) => "[??:??:??]".to_string(),
        }
    }

    pub fn log(&self, level: LogLevel, msg: &str) {
        if level < self.min_level {
            return;
        }

        let timestamp = Self::get_timestamp();
        eprintln!("[{}] [{}] {}", level, timestamp, msg);
    }
}

// Global logger (şimdilik basit static ama thread-safe için Mutex kullanılabilir)
// v0.3'te daha gelişmiş logger sistemi yapılabilir
static GLOBAL_LOGGER: std::sync::OnceLock<std::sync::Mutex<Option<LogLevel>>> =
    std::sync::OnceLock::new();

pub fn set_log_level(level: LogLevel) {
    let _ = GLOBAL_LOGGER.get_or_init(|| std::sync::Mutex::new(Some(level)));
}

pub fn log_message(level: LogLevel, msg: &str) {
    let min_level = GLOBAL_LOGGER
        .get_or_init(|| std::sync::Mutex::new(Some(LogLevel::Info)))
        .lock()
        .ok()
        .and_then(|l| *l)
        .unwrap_or(LogLevel::Info);

    if level >= min_level {
        let timestamp = MantleLogger::get_timestamp();
        eprintln!("[{}] [{}] {}", level, timestamp, msg);
    }
}

/// Log makroları
#[macro_export]
macro_rules! log_debug {
    ($($arg:tt)*) => {
        $crate::logger::log_message($crate::logger::LogLevel::Debug, &format!($($arg)*))
    };
}

#[macro_export]
macro_rules! log_info {
    ($($arg:tt)*) => {
        $crate::logger::log_message($crate::logger::LogLevel::Info, &format!($($arg)*))
    };
}

#[macro_export]
macro_rules! log_warn {
    ($($arg:tt)*) => {
        $crate::logger::log_message($crate::logger::LogLevel::Warn, &format!($($arg)*))
    };
}

#[macro_export]
macro_rules! log_error {
    ($($arg:tt)*) => {
        $crate::logger::log_message($crate::logger::LogLevel::Error, &format!($($arg)*))
    };
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_logger_creation() {
        let logger = MantleLogger::new(LogLevel::Info);
        assert_eq!(logger.min_level, LogLevel::Info);
    }

    #[test]
    fn test_log_timestamp_format() {
        let ts = MantleLogger::get_timestamp();
        // Format: HH:MM:SS.mmm
        assert!(ts.len() == 12); // "00:00:00.000" = 12 chars
        assert!(ts.contains(':'));
        assert!(ts.contains('.'));
    }

    #[test]
    fn test_debug_macro() {
        log_debug!("Debug test: {}", 42);
        // Makro çalışıyor, output'u manuel olarak kontrol et
    }

    #[test]
    fn test_info_macro() {
        log_info!("Info test");
    }

    #[test]
    fn test_warn_macro() {
        log_warn!("Warning test");
    }

    #[test]
    fn test_error_macro() {
        log_error!("Error test");
    }
}
