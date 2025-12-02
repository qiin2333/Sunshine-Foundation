//! Sunshine Tray - Rust implementation of the tray icon library
//!
//! This library provides a C-compatible API that mirrors the original tray library,
//! but uses the `tray-icon` Rust crate for the underlying implementation.

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

mod ffi;

use ffi::{tray, tray_menu};

use std::ffi::CStr;
use std::os::raw::{c_char, c_int};
use std::path::Path;
use std::sync::atomic::{AtomicBool, Ordering};

use image::ImageReader;
use muda::{Menu, MenuEvent, MenuItem, PredefinedMenuItem, Submenu};
use once_cell::sync::OnceCell;
use parking_lot::Mutex;
use tray_icon::{Icon, TrayIcon, TrayIconBuilder};

#[cfg(target_os = "windows")]
mod platform {
    pub use windows_sys::Win32::UI::WindowsAndMessaging::{
        DispatchMessageW, GetMessageW, PeekMessageW, PostQuitMessage, TranslateMessage, 
        MSG, PM_REMOVE, WM_QUIT,
    };
}

#[cfg(target_os = "linux")]
mod platform {
    pub use glib::MainContext;
    pub use gtk::prelude::*;
}

#[cfg(target_os = "macos")]
mod platform {
    pub use objc2_app_kit::NSApplication;
    pub use objc2_foundation::{NSDate, NSDefaultRunLoopMode, NSRunLoop};
}

/// Global tray state
struct TrayState {
    /// The tray icon instance
    icon: TrayIcon,
    /// Menu items with their callbacks
    menu_items: Vec<MenuItemInfo>,
    /// Pointer to the original C tray struct (for accessing callbacks)
    c_tray: *mut tray,
}

/// Information about a menu item
struct MenuItemInfo {
    /// The menu item ID
    id: muda::MenuId,
    /// Pointer to the original C menu item
    c_menu: *const tray_menu,
}

// Safety: TrayState is only accessed from the main thread
unsafe impl Send for TrayState {}
unsafe impl Sync for TrayState {}

/// Global state storage
static TRAY_STATE: OnceCell<Mutex<Option<TrayState>>> = OnceCell::new();

/// Flag to control the event loop
static SHOULD_EXIT: AtomicBool = AtomicBool::new(false);

/// Convert a C string pointer to a Rust string slice
unsafe fn c_str_to_str<'a>(ptr: *const c_char) -> Option<&'a str> {
    if ptr.is_null() {
        return None;
    }
    CStr::from_ptr(ptr).to_str().ok()
}

/// Load an icon from file path
fn load_icon_from_path(path: &str) -> Option<Icon> {
    let path = Path::new(path);

    // Try to load the image
    let img = match ImageReader::open(path) {
        Ok(reader) => match reader.decode() {
            Ok(img) => img.into_rgba8(),
            Err(e) => {
                log::error!("Failed to decode icon image: {}", e);
                return None;
            }
        },
        Err(e) => {
            log::error!("Failed to open icon file '{}': {}", path.display(), e);
            return None;
        }
    };

    let (width, height) = img.dimensions();
    let rgba = img.into_raw();

    match Icon::from_rgba(rgba, width, height) {
        Ok(icon) => Some(icon),
        Err(e) => {
            log::error!("Failed to create icon: {}", e);
            None
        }
    }
}

#[cfg(target_os = "linux")]
fn load_icon_by_name(name: &str) -> Option<Icon> {
    // On Linux, icons are typically loaded by name from the icon theme
    // For now, we'll try some common paths
    let paths = [
        format!("/usr/share/icons/hicolor/256x256/apps/{}.png", name),
        format!("/usr/share/icons/hicolor/128x128/apps/{}.png", name),
        format!("/usr/share/icons/hicolor/64x64/apps/{}.png", name),
        format!("/usr/share/icons/hicolor/48x48/apps/{}.png", name),
        format!("/usr/share/pixmaps/{}.png", name),
    ];

    for path in &paths {
        if Path::new(path).exists() {
            if let Some(icon) = load_icon_from_path(path) {
                return Some(icon);
            }
        }
    }

    log::warn!("Could not find icon by name: {}", name);
    None
}

/// Load icon based on the icon path/name
fn load_icon(icon_str: &str) -> Option<Icon> {
    // Check if it's a file path
    if Path::new(icon_str).exists() {
        return load_icon_from_path(icon_str);
    }

    // On Linux, it might be an icon name
    #[cfg(target_os = "linux")]
    {
        return load_icon_by_name(icon_str);
    }

    #[cfg(not(target_os = "linux"))]
    {
        log::error!("Icon not found: {}", icon_str);
        None
    }
}

/// Build submenu recursively and append items directly to parent
unsafe fn build_submenu_items(submenu: &Submenu, menu_ptr: *const tray_menu, items: &mut Vec<MenuItemInfo>) {
    if menu_ptr.is_null() {
        return;
    }

    let mut current = menu_ptr;
    while !current.is_null() {
        let menu_item = &*current;

        // Check for null terminator
        if menu_item.text.is_null() {
            break;
        }

        let text = c_str_to_str(menu_item.text).unwrap_or("");

        // Check for separator
        if text == "-" {
            let _ = submenu.append(&PredefinedMenuItem::separator());
            current = current.add(1);
            continue;
        }

        // Check for nested submenu
        if !menu_item.submenu.is_null() {
            let nested_submenu = Submenu::new(text, true);
            build_submenu_items(&nested_submenu, menu_item.submenu, items);
            let _ = submenu.append(&nested_submenu);
        } else {
            // Regular menu item
            if menu_item.checkbox != 0 {
                // Checkbox item
                let check_item = muda::CheckMenuItem::new(text, true, menu_item.checked != 0, None);
                items.push(MenuItemInfo {
                    id: check_item.id().clone(),
                    c_menu: current,
                });
                let _ = submenu.append(&check_item);
            } else {
                // Normal item
                let normal_item = MenuItem::new(text, menu_item.disabled == 0, None);
                items.push(MenuItemInfo {
                    id: normal_item.id().clone(),
                    c_menu: current,
                });
                let _ = submenu.append(&normal_item);
            }
        }

        current = current.add(1);
    }
}

/// Build menu recursively from C tray_menu structure
unsafe fn build_menu(menu_ptr: *const tray_menu) -> (Menu, Vec<MenuItemInfo>) {
    let menu = Menu::new();
    let mut items = Vec::new();

    if menu_ptr.is_null() {
        return (menu, items);
    }

    let mut current = menu_ptr;
    while !current.is_null() {
        let menu_item = &*current;

        // Check for null terminator
        if menu_item.text.is_null() {
            break;
        }

        let text = c_str_to_str(menu_item.text).unwrap_or("");

        // Check for separator
        if text == "-" {
            let _ = menu.append(&PredefinedMenuItem::separator());
            current = current.add(1);
            continue;
        }

        // Check for submenu
        if !menu_item.submenu.is_null() {
            let submenu = Submenu::new(text, true);
            // Build submenu items directly into the submenu
            build_submenu_items(&submenu, menu_item.submenu, &mut items);
            let _ = menu.append(&submenu);
        } else {
            // Regular menu item
            if menu_item.checkbox != 0 {
                // Checkbox item
                let check_item = muda::CheckMenuItem::new(text, true, menu_item.checked != 0, None);
                items.push(MenuItemInfo {
                    id: check_item.id().clone(),
                    c_menu: current,
                });
                let _ = menu.append(&check_item);
            } else {
                // Normal item
                let normal_item = MenuItem::new(text, menu_item.disabled == 0, None);
                items.push(MenuItemInfo {
                    id: normal_item.id().clone(),
                    c_menu: current,
                });
                let _ = menu.append(&normal_item);
            }
        }

        current = current.add(1);
    }

    (menu, items)
}

/// Initialize the tray icon
///
/// # Safety
/// The caller must ensure that `t` points to a valid `tray` structure
/// that remains valid for the lifetime of the tray.
#[no_mangle]
pub unsafe extern "C" fn tray_init(t: *mut tray) -> c_int {
    if t.is_null() {
        return -1;
    }

    // Initialize the global state
    let _ = TRAY_STATE.get_or_init(|| Mutex::new(None));

    #[cfg(target_os = "linux")]
    {
        // Initialize GTK
        if gtk::init().is_err() {
            log::error!("Failed to initialize GTK");
            return -1;
        }
    }

    // Reset exit flag
    SHOULD_EXIT.store(false, Ordering::SeqCst);

    let tray_ref = &*t;

    // Load icon
    let icon = match c_str_to_str(tray_ref.icon) {
        Some(icon_path) => match load_icon(icon_path) {
            Some(i) => i,
            None => {
                log::error!("Failed to load tray icon");
                return -1;
            }
        },
        None => {
            log::error!("No icon path provided");
            return -1;
        }
    };

    // Get tooltip
    let tooltip = c_str_to_str(tray_ref.tooltip);

    // Build menu
    let (menu, menu_items) = build_menu(tray_ref.menu);

    // Create tray icon
    let mut builder = TrayIconBuilder::new().with_icon(icon).with_menu(Box::new(menu));

    if let Some(tip) = tooltip {
        builder = builder.with_tooltip(tip);
    }

    match builder.build() {
        Ok(tray_icon) => {
            let state = TrayState {
                icon: tray_icon,
                menu_items,
                c_tray: t,
            };

            if let Some(state_mutex) = TRAY_STATE.get() {
                *state_mutex.lock() = Some(state);
            }

            0
        }
        Err(e) => {
            log::error!("Failed to create tray icon: {}", e);
            -1
        }
    }
}

/// Update the tray icon and menu
///
/// # Safety
/// The caller must ensure that `t` points to a valid `tray` structure.
#[no_mangle]
pub unsafe extern "C" fn tray_update(t: *mut tray) {
    if t.is_null() {
        return;
    }

    let state_mutex = match TRAY_STATE.get() {
        Some(s) => s,
        None => return,
    };

    let mut state_guard = state_mutex.lock();
    let state = match state_guard.as_mut() {
        Some(s) => s,
        None => return,
    };

    let tray_ref = &*t;

    // Update icon if changed
    if let Some(icon_path) = c_str_to_str(tray_ref.icon) {
        if let Some(new_icon) = load_icon(icon_path) {
            let _ = state.icon.set_icon(Some(new_icon));
        }
    }

    // Update tooltip if changed
    if let Some(tip) = c_str_to_str(tray_ref.tooltip) {
        let _ = state.icon.set_tooltip(Some(tip));
    }

    // Check for notification
    if !tray_ref.notification_title.is_null() && !tray_ref.notification_text.is_null() {
        let title = c_str_to_str(tray_ref.notification_title).unwrap_or("");
        let text = c_str_to_str(tray_ref.notification_text).unwrap_or("");

        if !title.is_empty() || !text.is_empty() {
            // Show notification - tray-icon doesn't have built-in notification support
            // We would need to use a separate notification library here
            log::info!("Notification: {} - {}", title, text);

            #[cfg(target_os = "windows")]
            {
                // On Windows, we can use the tray icon's balloon notification
                // This requires accessing the underlying Windows API
                // For now, just log
            }

            #[cfg(target_os = "linux")]
            {
                // On Linux, we can use libnotify
                // For now, just log
            }
        }
    }

    // Rebuild menu (simpler approach - recreate menu on update)
    let (menu, menu_items) = build_menu(tray_ref.menu);
    let _ = state.icon.set_menu(Some(Box::new(menu)));
    state.menu_items = menu_items;
    state.c_tray = t;
}

/// Process menu events and invoke callbacks
unsafe fn process_menu_event(state: &TrayState, event: &MenuEvent) {
    for item_info in &state.menu_items {
        if item_info.id == event.id {
            let menu_item = &*item_info.c_menu;
            if let Some(cb) = menu_item.cb {
                // Call the C callback
                cb(item_info.c_menu as *mut tray_menu);
            }
            break;
        }
    }
}

/// Run one iteration of the UI loop
///
/// # Arguments
/// * `blocking` - If non-zero, block until an event is available
///
/// # Returns
/// * 0 on success
/// * -1 if `tray_exit()` was called
#[no_mangle]
pub extern "C" fn tray_loop(blocking: c_int) -> c_int {
    if SHOULD_EXIT.load(Ordering::SeqCst) {
        return -1;
    }

    #[cfg(target_os = "windows")]
    {
        use platform::*;

        unsafe {
            let mut msg: MSG = std::mem::zeroed();

            if blocking != 0 {
                // Blocking mode - wait for message
                if GetMessageW(&mut msg, std::ptr::null_mut(), 0, 0) <= 0 {
                    return -1;
                }
            } else {
                // Non-blocking mode - peek for message
                if PeekMessageW(&mut msg, std::ptr::null_mut(), 0, 0, PM_REMOVE) == 0 {
                    return 0;
                }
            }

            if msg.message == WM_QUIT {
                return -1;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        // Process menu events
        if let Ok(event) = MenuEvent::receiver().try_recv() {
            if let Some(state_mutex) = TRAY_STATE.get() {
                let state_guard = state_mutex.lock();
                if let Some(ref state) = *state_guard {
                    unsafe {
                        process_menu_event(state, &event);
                    }
                }
            }
        }

        if SHOULD_EXIT.load(Ordering::SeqCst) {
            return -1;
        }

        0
    }

    #[cfg(target_os = "linux")]
    {
        // Process GTK events
        if blocking != 0 {
            // Process one event, blocking if necessary
            while gtk::events_pending() {
                gtk::main_iteration();
            }

            // Wait a bit if no events
            std::thread::sleep(std::time::Duration::from_millis(100));
        } else {
            // Process pending events without blocking
            while gtk::events_pending() {
                gtk::main_iteration();
            }
        }

        // Process menu events
        if let Ok(event) = MenuEvent::receiver().try_recv() {
            if let Some(state_mutex) = TRAY_STATE.get() {
                let state_guard = state_mutex.lock();
                if let Some(ref state) = *state_guard {
                    unsafe {
                        process_menu_event(state, &event);
                    }
                }
            }
        }

        if SHOULD_EXIT.load(Ordering::SeqCst) {
            return -1;
        }

        0
    }

    #[cfg(target_os = "macos")]
    {
        use objc2::rc::autoreleasepool;
        use platform::*;

        autoreleasepool(|_| {
            unsafe {
                let app = NSApplication::sharedApplication();
                let run_loop = NSRunLoop::currentRunLoop();

                if blocking != 0 {
                    // Run until a date in the future (blocking)
                    let date = NSDate::dateWithTimeIntervalSinceNow(0.1);
                    run_loop.runUntilDate(&date);
                } else {
                    // Run once without blocking
                    let date = NSDate::dateWithTimeIntervalSinceNow(0.0);
                    run_loop.runUntilDate(&date);
                }
            }
        });

        // Process menu events
        if let Ok(event) = MenuEvent::receiver().try_recv() {
            if let Some(state_mutex) = TRAY_STATE.get() {
                let state_guard = state_mutex.lock();
                if let Some(ref state) = *state_guard {
                    unsafe {
                        process_menu_event(state, &event);
                    }
                }
            }
        }

        if SHOULD_EXIT.load(Ordering::SeqCst) {
            return -1;
        }

        0
    }

    #[cfg(not(any(target_os = "windows", target_os = "linux", target_os = "macos")))]
    {
        log::error!("Unsupported platform");
        -1
    }
}

/// Terminate the UI loop
#[no_mangle]
pub extern "C" fn tray_exit() {
    SHOULD_EXIT.store(true, Ordering::SeqCst);

    #[cfg(target_os = "windows")]
    {
        unsafe {
            platform::PostQuitMessage(0);
        }
    }

    #[cfg(target_os = "linux")]
    {
        gtk::main_quit();
    }

    // Clean up state
    if let Some(state_mutex) = TRAY_STATE.get() {
        let mut state_guard = state_mutex.lock();
        *state_guard = None;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_c_str_to_str() {
        unsafe {
            assert_eq!(c_str_to_str(std::ptr::null()), None);

            let s = std::ffi::CString::new("hello").unwrap();
            assert_eq!(c_str_to_str(s.as_ptr()), Some("hello"));
        }
    }
}
