//! Sunshine Tray - Rust implementation of the system tray
//!
//! This library provides a complete system tray implementation with:
//! - Multi-language support (Chinese, English, Japanese)
//! - Menu management
//! - Notification support
//! - Icon management

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

pub mod i18n;
pub mod actions;

use std::ffi::CStr;
use std::os::raw::{c_char, c_int};
use std::path::Path;
use std::sync::atomic::{AtomicBool, Ordering};

use image::ImageReader;
use muda::{Menu, MenuEvent, MenuId, MenuItem, PredefinedMenuItem, Submenu, CheckMenuItem};
use once_cell::sync::OnceCell;
use parking_lot::Mutex;
use tray_icon::{Icon, TrayIcon, TrayIconBuilder};

use i18n::{StringKey, get_string, set_locale_str};
use actions::{MenuAction, trigger_action, register_callback, ActionCallback, open_url, urls};

#[cfg(target_os = "windows")]
use windows_sys::Win32::UI::WindowsAndMessaging::{
    DispatchMessageW, GetMessageW, PeekMessageW, PostQuitMessage, TranslateMessage, 
    MSG, WM_QUIT, PM_REMOVE,
};

/// Tray state
#[allow(dead_code)]  // Fields are needed for lifetime management
struct TrayState {
    icon: TrayIcon,
    menu: Menu,
    // Menu item IDs for dynamic updates
    vdd_toggle_id: MenuId,
    // Submenu references for language
    config_submenu: Submenu,
    language_submenu: Submenu,
    help_submenu: Submenu,
    // All menu items for rebuilding
    menu_items: MenuItems,
}

struct MenuItems {
    open_sunshine: MenuItem,
    vdd_toggle: CheckMenuItem,
    import_config: MenuItem,
    export_config: MenuItem,
    reset_config: MenuItem,
    lang_chinese: MenuItem,
    lang_english: MenuItem,
    lang_japanese: MenuItem,
    star_project: MenuItem,
    donate_yundi339: MenuItem,
    donate_qiin: MenuItem,
    #[cfg(target_os = "windows")]
    reset_display: MenuItem,
    restart: MenuItem,
    quit: MenuItem,
}

// Safety: TrayState is only accessed from the main thread
unsafe impl Send for TrayState {}
unsafe impl Sync for TrayState {}

/// Global state
static TRAY_STATE: OnceCell<Mutex<Option<TrayState>>> = OnceCell::new();
static SHOULD_EXIT: AtomicBool = AtomicBool::new(false);

/// Icon paths storage
static ICON_PATHS: OnceCell<IconPaths> = OnceCell::new();

struct IconPaths {
    normal: String,
    playing: String,
    pausing: String,
    locked: String,
}

/// Convert C string to Rust string
unsafe fn c_str_to_string(ptr: *const c_char) -> Option<String> {
    if ptr.is_null() {
        return None;
    }
    CStr::from_ptr(ptr).to_str().ok().map(|s| s.to_string())
}

/// Load icon from file path
fn load_icon_from_path(path: &str) -> Option<Icon> {
    let path = Path::new(path);
    
    let img = match ImageReader::open(path) {
        Ok(reader) => match reader.decode() {
            Ok(img) => img.into_rgba8(),
            Err(e) => {
                eprintln!("Failed to decode icon: {}", e);
                return None;
            }
        },
        Err(e) => {
            eprintln!("Failed to open icon '{}': {}", path.display(), e);
            return None;
        }
    };

    let (width, height) = img.dimensions();
    let rgba = img.into_raw();

    Icon::from_rgba(rgba, width, height).ok()
}

#[cfg(target_os = "linux")]
fn load_icon_by_name(name: &str) -> Option<Icon> {
    let paths = [
        format!("/usr/share/icons/hicolor/256x256/apps/{}.png", name),
        format!("/usr/share/icons/hicolor/128x128/apps/{}.png", name),
        format!("/usr/share/icons/hicolor/64x64/apps/{}.png", name),
        format!("/usr/share/pixmaps/{}.png", name),
    ];

    for path in &paths {
        if Path::new(path).exists() {
            if let Some(icon) = load_icon_from_path(path) {
                return Some(icon);
            }
        }
    }
    None
}

/// Load icon (handles both path and name)
fn load_icon(icon_str: &str) -> Option<Icon> {
    if Path::new(icon_str).exists() {
        return load_icon_from_path(icon_str);
    }

    #[cfg(target_os = "linux")]
    {
        return load_icon_by_name(icon_str);
    }

    #[cfg(not(target_os = "linux"))]
    {
        eprintln!("Icon not found: {}", icon_str);
        None
    }
}

/// Build the tray menu with current language
fn build_menu() -> (Menu, MenuItems, Submenu, Submenu, Submenu, MenuId) {
    let menu = Menu::new();
    
    // Open Sunshine
    let open_sunshine = MenuItem::new(get_string(StringKey::OpenSunshine), true, None);
    let _ = menu.append(&open_sunshine);
    
    // Separator
    let _ = menu.append(&PredefinedMenuItem::separator());
    
    // VDD Monitor Toggle (checkbox)
    let vdd_toggle = CheckMenuItem::new(get_string(StringKey::VddMonitorToggle), true, false, None);
    let vdd_toggle_id = vdd_toggle.id().clone();
    let _ = menu.append(&vdd_toggle);
    
    // Separator
    let _ = menu.append(&PredefinedMenuItem::separator());
    
    // Configuration submenu
    let import_config = MenuItem::new(get_string(StringKey::ImportConfig), true, None);
    let export_config = MenuItem::new(get_string(StringKey::ExportConfig), true, None);
    let reset_config = MenuItem::new(get_string(StringKey::ResetToDefault), true, None);
    
    let config_submenu = Submenu::new(get_string(StringKey::Configuration), true);
    let _ = config_submenu.append(&import_config);
    let _ = config_submenu.append(&export_config);
    let _ = config_submenu.append(&reset_config);
    let _ = menu.append(&config_submenu);
    
    // Separator
    let _ = menu.append(&PredefinedMenuItem::separator());
    
    // Language submenu
    let lang_chinese = MenuItem::new(get_string(StringKey::Chinese), true, None);
    let lang_english = MenuItem::new(get_string(StringKey::English), true, None);
    let lang_japanese = MenuItem::new(get_string(StringKey::Japanese), true, None);
    
    let language_submenu = Submenu::new(get_string(StringKey::Language), true);
    let _ = language_submenu.append(&lang_chinese);
    let _ = language_submenu.append(&lang_english);
    let _ = language_submenu.append(&lang_japanese);
    let _ = menu.append(&language_submenu);
    
    // Separator
    let _ = menu.append(&PredefinedMenuItem::separator());
    
    // Star Project
    let star_project = MenuItem::new(get_string(StringKey::StarProject), true, None);
    let _ = menu.append(&star_project);
    
    // Help Us submenu
    let donate_yundi339 = MenuItem::new(get_string(StringKey::DeveloperYundi339), true, None);
    let donate_qiin = MenuItem::new(get_string(StringKey::DeveloperQiin), true, None);
    
    let help_submenu = Submenu::new(get_string(StringKey::HelpUs), true);
    let _ = help_submenu.append(&donate_yundi339);
    let _ = help_submenu.append(&donate_qiin);
    let _ = menu.append(&help_submenu);
    
    // Separator
    let _ = menu.append(&PredefinedMenuItem::separator());
    
    // Windows-specific: Reset Display Device Config
    #[cfg(target_os = "windows")]
    let reset_display = {
        let item = MenuItem::new(get_string(StringKey::ResetDisplayDeviceConfig), true, None);
        let _ = menu.append(&item);
        item
    };
    
    // Restart
    let restart = MenuItem::new(get_string(StringKey::Restart), true, None);
    let _ = menu.append(&restart);
    
    // Quit
    let quit = MenuItem::new(get_string(StringKey::Quit), true, None);
    let _ = menu.append(&quit);
    
    let menu_items = MenuItems {
        open_sunshine,
        vdd_toggle,
        import_config,
        export_config,
        reset_config,
        lang_chinese,
        lang_english,
        lang_japanese,
        star_project,
        donate_yundi339,
        donate_qiin,
        #[cfg(target_os = "windows")]
        reset_display,
        restart,
        quit,
    };
    
    (menu, menu_items, config_submenu, language_submenu, help_submenu, vdd_toggle_id)
}

/// Handle menu events
fn handle_menu_event(event: &MenuEvent, state: &TrayState) {
    let items = &state.menu_items;
    
    if event.id == items.open_sunshine.id() {
        trigger_action(MenuAction::OpenUI);
    } else if event.id == items.vdd_toggle.id() {
        trigger_action(MenuAction::ToggleVddMonitor);
    } else if event.id == items.import_config.id() {
        trigger_action(MenuAction::ImportConfig);
    } else if event.id == items.export_config.id() {
        trigger_action(MenuAction::ExportConfig);
    } else if event.id == items.reset_config.id() {
        trigger_action(MenuAction::ResetConfig);
    } else if event.id == items.lang_chinese.id() {
        set_locale_str("zh");
        trigger_action(MenuAction::LanguageChinese);
        update_menu_texts();
    } else if event.id == items.lang_english.id() {
        set_locale_str("en");
        trigger_action(MenuAction::LanguageEnglish);
        update_menu_texts();
    } else if event.id == items.lang_japanese.id() {
        set_locale_str("ja");
        trigger_action(MenuAction::LanguageJapanese);
        update_menu_texts();
    } else if event.id == items.star_project.id() {
        open_url(urls::GITHUB_PROJECT);
        trigger_action(MenuAction::StarProject);
    } else if event.id == items.donate_yundi339.id() {
        open_url(urls::DONATE_YUNDI339);
        trigger_action(MenuAction::DonateYundi339);
    } else if event.id == items.donate_qiin.id() {
        open_url(urls::DONATE_QIIN);
        trigger_action(MenuAction::DonateQiin);
    } else if event.id == items.restart.id() {
        trigger_action(MenuAction::Restart);
    } else if event.id == items.quit.id() {
        trigger_action(MenuAction::Quit);
    }
    #[cfg(target_os = "windows")]
    if event.id == items.reset_display.id() {
        trigger_action(MenuAction::ResetDisplayDeviceConfig);
    }
}

/// Update menu texts after language change
fn update_menu_texts() {
    if let Some(state_mutex) = TRAY_STATE.get() {
        let state_guard = state_mutex.lock();
        if let Some(ref state) = *state_guard {
            // Update menu item texts
            state.menu_items.open_sunshine.set_text(get_string(StringKey::OpenSunshine));
            state.menu_items.vdd_toggle.set_text(get_string(StringKey::VddMonitorToggle));
            state.menu_items.import_config.set_text(get_string(StringKey::ImportConfig));
            state.menu_items.export_config.set_text(get_string(StringKey::ExportConfig));
            state.menu_items.reset_config.set_text(get_string(StringKey::ResetToDefault));
            state.menu_items.lang_chinese.set_text(get_string(StringKey::Chinese));
            state.menu_items.lang_english.set_text(get_string(StringKey::English));
            state.menu_items.lang_japanese.set_text(get_string(StringKey::Japanese));
            state.menu_items.star_project.set_text(get_string(StringKey::StarProject));
            state.menu_items.donate_yundi339.set_text(get_string(StringKey::DeveloperYundi339));
            state.menu_items.donate_qiin.set_text(get_string(StringKey::DeveloperQiin));
            #[cfg(target_os = "windows")]
            state.menu_items.reset_display.set_text(get_string(StringKey::ResetDisplayDeviceConfig));
            state.menu_items.restart.set_text(get_string(StringKey::Restart));
            state.menu_items.quit.set_text(get_string(StringKey::Quit));

            // Update submenu texts
            state.config_submenu.set_text(get_string(StringKey::Configuration));
            state.language_submenu.set_text(get_string(StringKey::Language));
            state.help_submenu.set_text(get_string(StringKey::HelpUs));

            // Re-set the menu on the tray icon to ensure click events work after text update
            // This is necessary on Windows where menu updates don't automatically propagate
            let _ = state.icon.set_menu(Some(Box::new(state.menu.clone())));
        }
    }
}

// ============================================================================
// C FFI Interface
// ============================================================================

/// Initialize the tray with icon paths
/// 
/// # Arguments
/// * `icon_normal` - Path to normal icon
/// * `icon_playing` - Path to playing icon
/// * `icon_pausing` - Path to pausing icon
/// * `icon_locked` - Path to locked icon
/// * `tooltip` - Tooltip text
/// * `locale` - Initial locale (e.g., "zh", "en", "ja")
/// * `callback` - Callback function for menu actions
/// 
/// # Returns
/// 0 on success, -1 on error
#[no_mangle]
pub unsafe extern "C" fn tray_init_ex(
    icon_normal: *const c_char,
    icon_playing: *const c_char,
    icon_pausing: *const c_char,
    icon_locked: *const c_char,
    tooltip: *const c_char,
    locale: *const c_char,
    callback: ActionCallback,
) -> c_int {
    // Store icon paths
    let normal = c_str_to_string(icon_normal).unwrap_or_default();
    let playing = c_str_to_string(icon_playing).unwrap_or_default();
    let pausing = c_str_to_string(icon_pausing).unwrap_or_default();
    let locked = c_str_to_string(icon_locked).unwrap_or_default();
    
    let _ = ICON_PATHS.set(IconPaths {
        normal: normal.clone(),
        playing,
        pausing,
        locked,
    });
    
    // Set locale
    if let Some(loc) = c_str_to_string(locale) {
        set_locale_str(&loc);
    }
    
    // Register callback
    register_callback(callback);
    
    // Initialize global state
    let _ = TRAY_STATE.get_or_init(|| Mutex::new(None));
    
    // Reset exit flag
    SHOULD_EXIT.store(false, Ordering::SeqCst);
    
    // Load icon
    let icon = match load_icon(&normal) {
        Some(i) => i,
        None => {
            eprintln!("Failed to load tray icon");
            return -1;
        }
    };
    
    // Get tooltip
    let tooltip_str = c_str_to_string(tooltip).unwrap_or_else(|| "Sunshine".to_string());
    
    // Build menu
    let (menu, menu_items, config_submenu, language_submenu, help_submenu, vdd_toggle_id) = build_menu();
    
    // Create tray icon
    let tray_icon = match TrayIconBuilder::new()
        .with_icon(icon)
        .with_tooltip(&tooltip_str)
        .with_menu(Box::new(menu.clone()))
        .build()
    {
        Ok(t) => t,
        Err(e) => {
            eprintln!("Failed to create tray icon: {}", e);
            return -1;
        }
    };
    
    // Store state
    let state = TrayState {
        icon: tray_icon,
        menu,
        vdd_toggle_id,
        config_submenu,
        language_submenu,
        help_submenu,
        menu_items,
    };
    
    if let Some(state_mutex) = TRAY_STATE.get() {
        *state_mutex.lock() = Some(state);
    }
    
    0
}

/// Run one iteration of the event loop
/// 
/// # Arguments
/// * `blocking` - If non-zero, block until an event is available
/// 
/// # Returns
/// 0 on success, -1 if exit was requested
#[no_mangle]
pub extern "C" fn tray_loop(blocking: c_int) -> c_int {
    if SHOULD_EXIT.load(Ordering::SeqCst) {
        return -1;
    }

    #[cfg(target_os = "windows")]
    {
        unsafe {
            let mut msg: MSG = std::mem::zeroed();

            if blocking != 0 {
                if GetMessageW(&mut msg, std::ptr::null_mut(), 0, 0) <= 0 {
                    return -1;
                }
            } else {
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
                    handle_menu_event(&event, state);
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
        // GTK event loop
        while gtk::events_pending() {
            gtk::main_iteration();
        }

        if blocking != 0 {
            std::thread::sleep(std::time::Duration::from_millis(100));
        }

        // Process menu events
        if let Ok(event) = MenuEvent::receiver().try_recv() {
            if let Some(state_mutex) = TRAY_STATE.get() {
                let state_guard = state_mutex.lock();
                if let Some(ref state) = *state_guard {
                    handle_menu_event(&event, state);
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
        use objc2_foundation::{NSDate, NSRunLoop};

        autoreleasepool(|_| {
            unsafe {
                let run_loop = NSRunLoop::currentRunLoop();
                let interval = if blocking != 0 { 0.1 } else { 0.0 };
                let date = NSDate::dateWithTimeIntervalSinceNow(interval);
                run_loop.runUntilDate(&date);
            }
        });

        // Process menu events
        if let Ok(event) = MenuEvent::receiver().try_recv() {
            if let Some(state_mutex) = TRAY_STATE.get() {
                let state_guard = state_mutex.lock();
                if let Some(ref state) = *state_guard {
                    handle_menu_event(&event, state);
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
        -1
    }
}

/// Exit the tray event loop
#[no_mangle]
pub extern "C" fn tray_exit() {
    SHOULD_EXIT.store(true, Ordering::SeqCst);

    #[cfg(target_os = "windows")]
    unsafe {
        PostQuitMessage(0);
    }

    #[cfg(target_os = "linux")]
    {
        gtk::main_quit();
    }

    // Clean up state
    if let Some(state_mutex) = TRAY_STATE.get() {
        *state_mutex.lock() = None;
    }
}

/// Set the tray icon
/// 
/// # Arguments
/// * `icon_type` - 0=normal, 1=playing, 2=pausing, 3=locked
#[no_mangle]
pub extern "C" fn tray_set_icon(icon_type: c_int) {
    let icon_paths = match ICON_PATHS.get() {
        Some(p) => p,
        None => return,
    };
    
    let icon_path = match icon_type {
        0 => &icon_paths.normal,
        1 => &icon_paths.playing,
        2 => &icon_paths.pausing,
        3 => &icon_paths.locked,
        _ => &icon_paths.normal,
    };
    
    if let Some(icon) = load_icon(icon_path) {
        if let Some(state_mutex) = TRAY_STATE.get() {
            let state_guard = state_mutex.lock();
            if let Some(ref state) = *state_guard {
                let _ = state.icon.set_icon(Some(icon));
            }
        }
    }
}

/// Set the tray tooltip
#[no_mangle]
pub unsafe extern "C" fn tray_set_tooltip(tooltip: *const c_char) {
    if let Some(tip) = c_str_to_string(tooltip) {
        if let Some(state_mutex) = TRAY_STATE.get() {
            let state_guard = state_mutex.lock();
            if let Some(ref state) = *state_guard {
                let _ = state.icon.set_tooltip(Some(&tip));
            }
        }
    }
}

/// Update the VDD monitor toggle checkbox state
#[no_mangle]
pub extern "C" fn tray_set_vdd_checked(checked: c_int) {
    if let Some(state_mutex) = TRAY_STATE.get() {
        let state_guard = state_mutex.lock();
        if let Some(ref state) = *state_guard {
            state.menu_items.vdd_toggle.set_checked(checked != 0);
        }
    }
}

/// Set the VDD toggle menu item enabled state
#[no_mangle]
pub extern "C" fn tray_set_vdd_enabled(enabled: c_int) {
    if let Some(state_mutex) = TRAY_STATE.get() {
        let state_guard = state_mutex.lock();
        if let Some(ref state) = *state_guard {
            state.menu_items.vdd_toggle.set_enabled(enabled != 0);
        }
    }
}

/// Set the current locale
#[no_mangle]
pub unsafe extern "C" fn tray_set_locale(locale: *const c_char) {
    if let Some(loc) = c_str_to_string(locale) {
        set_locale_str(&loc);
        update_menu_texts();
    }
}

/// Show a notification (placeholder - needs platform-specific implementation)
#[no_mangle]
pub unsafe extern "C" fn tray_show_notification(
    title: *const c_char,
    text: *const c_char,
    _icon_type: c_int,
) {
    let title_str = c_str_to_string(title).unwrap_or_default();
    let text_str = c_str_to_string(text).unwrap_or_default();
    
    // Log for now - proper notification support needs platform-specific implementation
    eprintln!("Notification: {} - {}", title_str, text_str);
}

// ============================================================================
// Legacy C API compatibility (for existing C++ code)
// ============================================================================

/// Legacy tray structure (for compatibility)
#[repr(C)]
pub struct tray {
    pub icon: *const c_char,
    pub tooltip: *const c_char,
    pub notification_icon: *const c_char,
    pub notification_text: *const c_char,
    pub notification_title: *const c_char,
    pub notification_cb: Option<extern "C" fn()>,
    pub menu: *mut tray_menu,
    pub iconPathCount: c_int,
    pub allIconPaths: [*const c_char; 4],
}

/// Legacy tray menu structure (for compatibility)
#[repr(C)]
pub struct tray_menu {
    pub text: *const c_char,
    pub disabled: c_int,
    pub checked: c_int,
    pub checkbox: c_int,
    pub cb: Option<extern "C" fn(*mut tray_menu)>,
    pub context: *mut std::ffi::c_void,
    pub submenu: *mut tray_menu,
}

/// Legacy tray_init - not recommended, use tray_init_ex instead
#[no_mangle]
pub unsafe extern "C" fn tray_init(_tray: *mut tray) -> c_int {
    eprintln!("Warning: tray_init is deprecated, use tray_init_ex instead");
    -1
}

/// Legacy tray_update - partially supported
#[no_mangle]
pub unsafe extern "C" fn tray_update(t: *mut tray) {
    if t.is_null() {
        return;
    }
    
    let tray_ref = &*t;
    
    // Update icon if changed
    if !tray_ref.icon.is_null() {
        if let Some(icon_str) = c_str_to_string(tray_ref.icon) {
            if let Some(icon) = load_icon(&icon_str) {
                if let Some(state_mutex) = TRAY_STATE.get() {
                    let state_guard = state_mutex.lock();
                    if let Some(ref state) = *state_guard {
                        let _ = state.icon.set_icon(Some(icon));
                    }
                }
            }
        }
    }
    
    // Update tooltip
    if !tray_ref.tooltip.is_null() {
        if let Some(tip) = c_str_to_string(tray_ref.tooltip) {
            if let Some(state_mutex) = TRAY_STATE.get() {
                let state_guard = state_mutex.lock();
                if let Some(ref state) = *state_guard {
                    let _ = state.icon.set_tooltip(Some(&tip));
                }
            }
        }
    }
    
    // Handle notifications
    if !tray_ref.notification_title.is_null() && !tray_ref.notification_text.is_null() {
        let title = c_str_to_string(tray_ref.notification_title).unwrap_or_default();
        let text = c_str_to_string(tray_ref.notification_text).unwrap_or_default();
        if !title.is_empty() || !text.is_empty() {
            tray_show_notification(
                tray_ref.notification_title,
                tray_ref.notification_text,
                0,
            );
        }
    }
}
