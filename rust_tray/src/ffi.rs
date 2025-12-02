//! FFI definitions matching the original tray.h header
//!
//! These structures are manually defined to match the C header file exactly,
//! avoiding the need for bindgen and LLVM/Clang dependencies.

use std::os::raw::{c_char, c_int, c_void};

/// Tray menu item callback type
pub type tray_menu_cb = Option<unsafe extern "C" fn(*mut tray_menu)>;

/// Notification callback type
pub type notification_cb = Option<unsafe extern "C" fn()>;

/// Tray menu item.
///
/// Represents a single item in the tray menu. Items can be:
/// - Regular menu items with text and callback
/// - Separators (text = "-")
/// - Checkbox items (checkbox = 1)
/// - Submenus (submenu != null)
#[repr(C)]
#[derive(Debug)]
pub struct tray_menu {
    /// Text to display. Use "-" for separator.
    pub text: *const c_char,
    /// Whether the item is disabled (grayed out).
    pub disabled: c_int,
    /// Whether the item is checked (for checkbox items).
    pub checked: c_int,
    /// Whether the item is a checkbox.
    pub checkbox: c_int,
    /// Callback to invoke when the item is clicked.
    pub cb: tray_menu_cb,
    /// Context pointer passed to the callback.
    pub context: *mut c_void,
    /// Pointer to submenu items (null-terminated array).
    pub submenu: *mut tray_menu,
}

/// Tray icon.
///
/// Main tray structure containing icon, tooltip, menu, and notification data.
#[repr(C)]
#[derive(Debug)]
pub struct tray {
    /// Icon path/name to display.
    pub icon: *const c_char,
    /// Tooltip text to display on hover.
    pub tooltip: *const c_char,
    /// Icon for notifications.
    pub notification_icon: *const c_char,
    /// Notification message text.
    pub notification_text: *const c_char,
    /// Notification title.
    pub notification_title: *const c_char,
    /// Callback when notification is clicked.
    pub notification_cb: notification_cb,
    /// Menu items.
    pub menu: *mut tray_menu,
    /// Number of icon paths in allIconPaths.
    pub iconPathCount: c_int,
    /// Array of all possible icon paths (flexible array member).
    /// Note: In C, this is `const char *allIconPaths[]`, which is a flexible array member.
    /// We represent it as a pointer to the first element.
    pub allIconPaths: [*const c_char; 0],
}

impl Default for tray_menu {
    fn default() -> Self {
        Self {
            text: std::ptr::null(),
            disabled: 0,
            checked: 0,
            checkbox: 0,
            cb: None,
            context: std::ptr::null_mut(),
            submenu: std::ptr::null_mut(),
        }
    }
}

impl Default for tray {
    fn default() -> Self {
        Self {
            icon: std::ptr::null(),
            tooltip: std::ptr::null(),
            notification_icon: std::ptr::null(),
            notification_text: std::ptr::null(),
            notification_title: std::ptr::null(),
            notification_cb: None,
            menu: std::ptr::null_mut(),
            iconPathCount: 0,
            allIconPaths: [],
        }
    }
}
