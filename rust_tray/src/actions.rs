//! Menu actions module
//! 
//! Defines all menu actions and their identifiers.
//! C++ side will register callbacks for these actions.

use std::sync::RwLock;
use once_cell::sync::Lazy;

/// Menu action identifiers
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
#[repr(u32)]
pub enum MenuAction {
    OpenUI = 1,
    ToggleVddMonitor = 2,
    ImportConfig = 3,
    ExportConfig = 4,
    ResetConfig = 5,
    LanguageChinese = 6,
    LanguageEnglish = 7,
    LanguageJapanese = 8,
    StarProject = 9,
    DonateYundi339 = 10,
    DonateQiin = 11,
    ResetDisplayDeviceConfig = 12,
    Restart = 13,
    Quit = 14,
    NotificationClicked = 15,
}

impl TryFrom<u32> for MenuAction {
    type Error = ();
    
    fn try_from(value: u32) -> Result<Self, Self::Error> {
        match value {
            1 => Ok(MenuAction::OpenUI),
            2 => Ok(MenuAction::ToggleVddMonitor),
            3 => Ok(MenuAction::ImportConfig),
            4 => Ok(MenuAction::ExportConfig),
            5 => Ok(MenuAction::ResetConfig),
            6 => Ok(MenuAction::LanguageChinese),
            7 => Ok(MenuAction::LanguageEnglish),
            8 => Ok(MenuAction::LanguageJapanese),
            9 => Ok(MenuAction::StarProject),
            10 => Ok(MenuAction::DonateYundi339),
            11 => Ok(MenuAction::DonateQiin),
            12 => Ok(MenuAction::ResetDisplayDeviceConfig),
            13 => Ok(MenuAction::Restart),
            14 => Ok(MenuAction::Quit),
            15 => Ok(MenuAction::NotificationClicked),
            _ => Err(()),
        }
    }
}

/// Callback function type for menu actions
pub type ActionCallback = extern "C" fn(action: u32);

/// Global callback storage
static ACTION_CALLBACK: Lazy<RwLock<Option<ActionCallback>>> = Lazy::new(|| RwLock::new(None));

/// Register the callback for menu actions
pub fn register_callback(callback: ActionCallback) {
    *ACTION_CALLBACK.write().unwrap() = Some(callback);
}

/// Trigger a menu action
pub fn trigger_action(action: MenuAction) {
    if let Some(callback) = *ACTION_CALLBACK.read().unwrap() {
        callback(action as u32);
    }
}

/// URLs for opening in browser
pub mod urls {
    pub const GITHUB_PROJECT: &str = "https://github.com/qiin2333/Sunshine-Foundation";
    pub const DONATE_YUNDI339: &str = "https://www.ifdian.net/a/Yundi339";
    pub const DONATE_QIIN: &str = "https://www.ifdian.net/a/qiin2333";
}

/// Open URL in default browser
#[cfg(target_os = "windows")]
pub fn open_url(url: &str) {
    use std::ffi::OsStr;
    use std::os::windows::ffi::OsStrExt;
    use std::ptr::null_mut;
    
    let wide_url: Vec<u16> = OsStr::new(url)
        .encode_wide()
        .chain(std::iter::once(0))
        .collect();
    
    unsafe {
        windows_sys::Win32::UI::Shell::ShellExecuteW(
            null_mut(),
            ['o' as u16, 'p' as u16, 'e' as u16, 'n' as u16, 0].as_ptr(),
            wide_url.as_ptr(),
            null_mut(),
            null_mut(),
            windows_sys::Win32::UI::WindowsAndMessaging::SW_SHOWNORMAL as i32,
        );
    }
}

#[cfg(target_os = "linux")]
pub fn open_url(url: &str) {
    let _ = std::process::Command::new("xdg-open")
        .arg(url)
        .spawn();
}

#[cfg(target_os = "macos")]
pub fn open_url(url: &str) {
    let _ = std::process::Command::new("open")
        .arg(url)
        .spawn();
}
