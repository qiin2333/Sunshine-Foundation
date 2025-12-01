#ifndef QIIN_TABTIP_I18N_H
#define QIIN_TABTIP_I18N_H

#include <string>

namespace qiin_tabtip_i18n {
  // String key constants
  const std::string KEY_TOUCH_KEYBOARD_DISPLAYED = "touch_keyboard_displayed";
  const std::string KEY_TABTIP_NOT_FOUND = "tabtip_not_found";
  const std::string KEY_CANNOT_DISPLAY_KEYBOARD = "cannot_display_keyboard";
  const std::string KEY_ONSCREEN_KEYBOARD_DISPLAYED = "onscreen_keyboard_displayed";
  const std::string KEY_TOUCH_KEYBOARD_HIDDEN = "touch_keyboard_hidden";
  const std::string KEY_KEYBOARD_NOT_RUNNING = "keyboard_not_running";
  const std::string KEY_SYSTEM_DIAGNOSTIC_INFO = "system_diagnostic_info";
  const std::string KEY_WINDOWS_VERSION = "windows_version";
  const std::string KEY_TABTIP_PATH = "tabtip_path";
  const std::string KEY_TABTIP_EXISTS = "tabtip_exists";
  const std::string KEY_TABTIP_NOT_EXISTS = "tabtip_not_exists";
  const std::string KEY_REGISTRY_SETTINGS = "registry_settings";
  const std::string KEY_ENABLED = "enabled";
  const std::string KEY_DISABLED = "disabled";
  const std::string KEY_CHECKING_KEYBOARD_WINDOW = "checking_keyboard_window";
  const std::string KEY_FOUND_WIN10 = "found_win10";
  const std::string KEY_FOUND_WIN11 = "found_win11";
  const std::string KEY_NOT_FOUND = "not_found";
  const std::string KEY_VISIBILITY = "visibility";
  const std::string KEY_VISIBLE = "visible";
  const std::string KEY_HIDDEN = "hidden";
  const std::string KEY_CURRENT_KEYBOARD_STATE = "current_keyboard_state";
  const std::string KEY_CANNOT_DISPLAY_OSK = "cannot_display_osk";
  const std::string KEY_TOOL_TITLE = "tool_title";
  const std::string KEY_USAGE = "usage";
  const std::string KEY_OPTIONS = "options";
  const std::string KEY_SHOW_DESC = "show_desc";
  const std::string KEY_HIDE_DESC = "hide_desc";
  const std::string KEY_TOGGLE_DESC = "toggle_desc";
  const std::string KEY_OSK_DESC = "osk_desc";
  const std::string KEY_STATUS_DESC = "status_desc";
  const std::string KEY_DIAGNOSE_DESC = "diagnose_desc";
  const std::string KEY_HELP_DESC = "help_desc";
  const std::string KEY_EXAMPLES = "examples";
  const std::string KEY_EXAMPLE_TOGGLE = "example_toggle";
  const std::string KEY_EXAMPLE_SHOW = "example_show";
  const std::string KEY_EXAMPLE_OSK = "example_osk";
  const std::string KEY_EXAMPLE_DIAGNOSE = "example_diagnose";
  const std::string KEY_STATUS_VISIBLE = "status_visible";
  const std::string KEY_STATUS_HIDDEN = "status_hidden";
  const std::string KEY_UNKNOWN_COMMAND = "unknown_command";
  const std::string KEY_USE_HELP = "use_help";

  // Initialize locale (defaults to English)
  void init_locale(const std::string& locale = "en");
  
  // Get current locale
  std::string get_current_locale();
  
  // Get localized string
  std::wstring get_localized_string(const std::string& key);
  
  // Get localized string with format arguments
  std::wstring get_localized_string_fmt(const std::string& key, ...);
}

#endif // QIIN_TABTIP_I18N_H

