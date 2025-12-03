//! Dark mode support for Windows context menus
//!
//! This module provides the ability to enable dark mode for context menus
//! (including system tray menus) on Windows 10 1903+ and Windows 11.
//!
//! The implementation uses undocumented Windows APIs from uxtheme.dll:
//! - SetPreferredAppMode (ordinal 135) - Sets the app's preferred dark mode
//! - FlushMenuThemes (ordinal 136) - Refreshes menu theme cache

#[cfg(target_os = "windows")]
mod windows_impl {
    use windows_sys::Win32::System::LibraryLoader::{GetProcAddress, LoadLibraryW};

    /// Preferred app mode values
    #[repr(i32)]
    #[derive(Debug, Clone, Copy)]
    pub enum PreferredAppMode {
        /// Use system default (usually light)
        Default = 0,
        /// Allow dark mode (follow system setting)
        AllowDark = 1,
        /// Force dark mode
        ForceDark = 2,
        /// Force light mode
        ForceLight = 3,
    }

    // Function pointer types for undocumented APIs
    type SetPreferredAppModeFn = unsafe extern "system" fn(mode: i32) -> i32;
    type FlushMenuThemesFn = unsafe extern "system" fn();

    /// Set the preferred app mode for context menus
    ///
    /// This affects the appearance of context menus (including tray menus).
    /// Call this before creating any menus.
    ///
    /// # Arguments
    /// * `mode` - The preferred app mode
    ///
    /// # Returns
    /// The previous mode on success, or -1 if the API is not available
    pub fn set_preferred_app_mode(mode: PreferredAppMode) -> i32 {
        unsafe {
            // Load uxtheme.dll
            // "uxtheme" in UTF-16
            let dll: [u16; 8] = [0x75, 0x78, 0x74, 0x68, 0x65, 0x6d, 0x65, 0];
            let module = LoadLibraryW(dll.as_ptr());

            if module.is_null() {
                return -1;
            }

            // Get SetPreferredAppMode by ordinal 135
            let func = GetProcAddress(module, 135 as *const u8);

            match func {
                Some(f) => {
                    let set_mode: SetPreferredAppModeFn = std::mem::transmute(f);
                    set_mode(mode as i32)
                }
                None => -1,
            }
        }
    }

    /// Flush the menu theme cache
    ///
    /// Call this after changing the app mode to refresh existing menus.
    pub fn flush_menu_themes() {
        unsafe {
            let dll: [u16; 8] = [0x75, 0x78, 0x74, 0x68, 0x65, 0x6d, 0x65, 0];
            let module = LoadLibraryW(dll.as_ptr());

            if module.is_null() {
                return;
            }

            // Get FlushMenuThemes by ordinal 136
            let func = GetProcAddress(module, 136 as *const u8);

            if let Some(f) = func {
                let flush: FlushMenuThemesFn = std::mem::transmute(f);
                flush();
            }
        }
    }

    /// Enable dark mode for context menus (follow system setting)
    ///
    /// This is the recommended way to enable dark mode support.
    /// The menu will automatically follow the system's dark/light mode setting.
    pub fn enable_dark_mode() {
        set_preferred_app_mode(PreferredAppMode::AllowDark);
        flush_menu_themes();
    }

    /// Force dark mode for context menus
    ///
    /// This forces menus to use dark mode regardless of system setting.
    pub fn force_dark_mode() {
        set_preferred_app_mode(PreferredAppMode::ForceDark);
        flush_menu_themes();
    }

    /// Force light mode for context menus
    pub fn force_light_mode() {
        set_preferred_app_mode(PreferredAppMode::ForceLight);
        flush_menu_themes();
    }
}

#[cfg(target_os = "windows")]
pub use windows_impl::*;

// Stub implementations for non-Windows platforms
#[cfg(not(target_os = "windows"))]
pub fn enable_dark_mode() {}

#[cfg(not(target_os = "windows"))]
pub fn force_dark_mode() {}

#[cfg(not(target_os = "windows"))]
pub fn force_light_mode() {}
