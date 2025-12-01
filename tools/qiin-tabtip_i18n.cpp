#include "qiin-tabtip_i18n.h"
#include <map>
#include <cstdarg>
#include <cstdio>
#include <Windows.h>

namespace qiin_tabtip_i18n {
  static std::string current_locale = "en";

  // Default English strings
  const std::map<std::string, std::wstring> DEFAULT_STRINGS = {
    { KEY_TOUCH_KEYBOARD_DISPLAYED, L"✓ Touch keyboard displayed" },
    { KEY_TABTIP_NOT_FOUND, L"✗ TabTip.exe not found" },
    { KEY_CANNOT_DISPLAY_KEYBOARD, L"✗ Cannot display keyboard" },
    { KEY_ONSCREEN_KEYBOARD_DISPLAYED, L"✓ On-screen keyboard displayed" },
    { KEY_TOUCH_KEYBOARD_HIDDEN, L"✓ Touch keyboard hidden" },
    { KEY_KEYBOARD_NOT_RUNNING, L"Touch keyboard is not running or already hidden" },
    { KEY_SYSTEM_DIAGNOSTIC_INFO, L"=== System Diagnostic Information ===" },
    { KEY_WINDOWS_VERSION, L"Windows version: %d.%d" },
    { KEY_TABTIP_PATH, L"TabTip path: %s" },
    { KEY_TABTIP_EXISTS, L"TabTip.exe: ✓ Exists" },
    { KEY_TABTIP_NOT_EXISTS, L"TabTip.exe: ✗ Not found" },
    { KEY_REGISTRY_SETTINGS, L"Registry settings:" },
    { KEY_ENABLED, L"Enabled" },
    { KEY_DISABLED, L"Disabled" },
    { KEY_CHECKING_KEYBOARD_WINDOW, L"Checking keyboard window:" },
    { KEY_FOUND_WIN10, L"Found (Windows 10)" },
    { KEY_FOUND_WIN11, L"Found (Windows 11)" },
    { KEY_NOT_FOUND, L"Not found" },
    { KEY_VISIBILITY, L"Visibility" },
    { KEY_VISIBLE, L"Visible" },
    { KEY_HIDDEN, L"Hidden" },
    { KEY_CURRENT_KEYBOARD_STATE, L"Current keyboard state" },
    { KEY_CANNOT_DISPLAY_OSK, L"✗ Cannot display on-screen keyboard" },
    { KEY_TOOL_TITLE, L"Windows Virtual Touch Keyboard Tool" },
    { KEY_USAGE, L"Usage:" },
    { KEY_OPTIONS, L"Options:" },
    { KEY_SHOW_DESC, L"  show      - Show touch keyboard (TabTip)" },
    { KEY_HIDE_DESC, L"  hide      - Hide touch keyboard" },
    { KEY_TOGGLE_DESC, L"  toggle    - Toggle keyboard display state (default)" },
    { KEY_OSK_DESC, L"  osk       - Show on-screen keyboard (OSK)" },
    { KEY_STATUS_DESC, L"  status    - Check if keyboard is visible" },
    { KEY_DIAGNOSE_DESC, L"  diagnose  - Diagnose system environment" },
    { KEY_HELP_DESC, L"  help      - Show this help message" },
    { KEY_EXAMPLES, L"Examples:" },
    { KEY_EXAMPLE_TOGGLE, L"  qiin-tabtip              # Toggle keyboard state" },
    { KEY_EXAMPLE_SHOW, L"  qiin-tabtip show         # Show touch keyboard" },
    { KEY_EXAMPLE_OSK, L"  qiin-tabtip osk          # Show on-screen keyboard" },
    { KEY_EXAMPLE_DIAGNOSE, L"  qiin-tabtip diagnose     # Diagnose issues" },
    { KEY_STATUS_VISIBLE, L"Touch keyboard current: Visible" },
    { KEY_STATUS_HIDDEN, L"Touch keyboard current: Hidden" },
    { KEY_UNKNOWN_COMMAND, L"Unknown command: %s" },
    { KEY_USE_HELP, L"Use 'qiin-tabtip help' to see help" }
  };

  // Chinese strings
  const std::map<std::string, std::wstring> CHINESE_STRINGS = {
    { KEY_TOUCH_KEYBOARD_DISPLAYED, L"✓ 触摸键盘已显示" },
    { KEY_TABTIP_NOT_FOUND, L"✗ 找不到 TabTip.exe" },
    { KEY_CANNOT_DISPLAY_KEYBOARD, L"✗ 无法显示键盘" },
    { KEY_ONSCREEN_KEYBOARD_DISPLAYED, L"✓ 屏幕键盘已显示" },
    { KEY_TOUCH_KEYBOARD_HIDDEN, L"✓ 触摸键盘已隐藏" },
    { KEY_KEYBOARD_NOT_RUNNING, L"触摸键盘未运行或已隐藏" },
    { KEY_SYSTEM_DIAGNOSTIC_INFO, L"=== 系统诊断信息 ===" },
    { KEY_WINDOWS_VERSION, L"Windows 版本: %d.%d" },
    { KEY_TABTIP_PATH, L"TabTip 路径: %s" },
    { KEY_TABTIP_EXISTS, L"TabTip.exe: ✓ 存在" },
    { KEY_TABTIP_NOT_EXISTS, L"TabTip.exe: ✗ 不存在" },
    { KEY_REGISTRY_SETTINGS, L"注册表设置:" },
    { KEY_ENABLED, L"已启用" },
    { KEY_DISABLED, L"未启用" },
    { KEY_CHECKING_KEYBOARD_WINDOW, L"检查键盘窗口:" },
    { KEY_FOUND_WIN10, L"找到 (Windows 10)" },
    { KEY_FOUND_WIN11, L"找到 (Windows 11)" },
    { KEY_NOT_FOUND, L"未找到" },
    { KEY_VISIBILITY, L"可见性" },
    { KEY_VISIBLE, L"可见" },
    { KEY_HIDDEN, L"隐藏" },
    { KEY_CURRENT_KEYBOARD_STATE, L"当前键盘状态" },
    { KEY_CANNOT_DISPLAY_OSK, L"✗ 无法显示屏幕键盘" },
    { KEY_TOOL_TITLE, L"Windows 虚拟触摸键盘工具" },
    { KEY_USAGE, L"用法:" },
    { KEY_OPTIONS, L"选项:" },
    { KEY_SHOW_DESC, L"  show      - 显示触摸键盘 (TabTip)" },
    { KEY_HIDE_DESC, L"  hide      - 隐藏触摸键盘" },
    { KEY_TOGGLE_DESC, L"  toggle    - 切换键盘显示状态 (默认)" },
    { KEY_OSK_DESC, L"  osk       - 显示屏幕键盘 (OSK)" },
    { KEY_STATUS_DESC, L"  status    - 检查键盘是否可见" },
    { KEY_DIAGNOSE_DESC, L"  diagnose  - 诊断系统环境" },
    { KEY_HELP_DESC, L"  help      - 显示此帮助信息" },
    { KEY_EXAMPLES, L"示例:" },
    { KEY_EXAMPLE_TOGGLE, L"  qiin-tabtip              # 切换键盘状态" },
    { KEY_EXAMPLE_SHOW, L"  qiin-tabtip show         # 显示触摸键盘" },
    { KEY_EXAMPLE_OSK, L"  qiin-tabtip osk          # 显示屏幕键盘" },
    { KEY_EXAMPLE_DIAGNOSE, L"  qiin-tabtip diagnose     # 诊断问题" },
    { KEY_STATUS_VISIBLE, L"触摸键盘当前: 可见" },
    { KEY_STATUS_HIDDEN, L"触摸键盘当前: 隐藏" },
    { KEY_UNKNOWN_COMMAND, L"未知命令: %s" },
    { KEY_USE_HELP, L"使用 'qiin-tabtip help' 查看帮助" }
  };

  // Japanese strings
  const std::map<std::string, std::wstring> JAPANESE_STRINGS = {
    { KEY_TOUCH_KEYBOARD_DISPLAYED, L"✓ タッチキーボードが表示されました" },
    { KEY_TABTIP_NOT_FOUND, L"✗ TabTip.exe が見つかりません" },
    { KEY_CANNOT_DISPLAY_KEYBOARD, L"✗ キーボードを表示できません" },
    { KEY_ONSCREEN_KEYBOARD_DISPLAYED, L"✓ スクリーンキーボードが表示されました" },
    { KEY_TOUCH_KEYBOARD_HIDDEN, L"✓ タッチキーボードが非表示になりました" },
    { KEY_KEYBOARD_NOT_RUNNING, L"タッチキーボードは実行されていないか、既に非表示です" },
    { KEY_SYSTEM_DIAGNOSTIC_INFO, L"=== システム診断情報 ===" },
    { KEY_WINDOWS_VERSION, L"Windows バージョン: %d.%d" },
    { KEY_TABTIP_PATH, L"TabTip パス: %s" },
    { KEY_TABTIP_EXISTS, L"TabTip.exe: ✓ 存在します" },
    { KEY_TABTIP_NOT_EXISTS, L"TabTip.exe: ✗ 見つかりません" },
    { KEY_REGISTRY_SETTINGS, L"レジストリ設定:" },
    { KEY_ENABLED, L"有効" },
    { KEY_DISABLED, L"無効" },
    { KEY_CHECKING_KEYBOARD_WINDOW, L"キーボードウィンドウを確認中:" },
    { KEY_FOUND_WIN10, L"見つかりました (Windows 10)" },
    { KEY_FOUND_WIN11, L"見つかりました (Windows 11)" },
    { KEY_NOT_FOUND, L"見つかりません" },
    { KEY_VISIBILITY, L"表示状態" },
    { KEY_VISIBLE, L"表示" },
    { KEY_HIDDEN, L"非表示" },
    { KEY_CURRENT_KEYBOARD_STATE, L"現在のキーボード状態" },
    { KEY_CANNOT_DISPLAY_OSK, L"✗ スクリーンキーボードを表示できません" },
    { KEY_TOOL_TITLE, L"Windows 仮想タッチキーボードツール" },
    { KEY_USAGE, L"使用方法:" },
    { KEY_OPTIONS, L"オプション:" },
    { KEY_SHOW_DESC, L"  show      - タッチキーボードを表示 (TabTip)" },
    { KEY_HIDE_DESC, L"  hide      - タッチキーボードを非表示" },
    { KEY_TOGGLE_DESC, L"  toggle    - キーボード表示状態を切り替え (デフォルト)" },
    { KEY_OSK_DESC, L"  osk       - スクリーンキーボードを表示 (OSK)" },
    { KEY_STATUS_DESC, L"  status    - キーボードが表示されているか確認" },
    { KEY_DIAGNOSE_DESC, L"  diagnose  - システム環境を診断" },
    { KEY_HELP_DESC, L"  help      - このヘルプメッセージを表示" },
    { KEY_EXAMPLES, L"例:" },
    { KEY_EXAMPLE_TOGGLE, L"  qiin-tabtip              # キーボード状態を切り替え" },
    { KEY_EXAMPLE_SHOW, L"  qiin-tabtip show         # タッチキーボードを表示" },
    { KEY_EXAMPLE_OSK, L"  qiin-tabtip osk          # スクリーンキーボードを表示" },
    { KEY_EXAMPLE_DIAGNOSE, L"  qiin-tabtip diagnose     # 問題を診断" },
    { KEY_STATUS_VISIBLE, L"タッチキーボード現在: 表示" },
    { KEY_STATUS_HIDDEN, L"タッチキーボード現在: 非表示" },
    { KEY_UNKNOWN_COMMAND, L"不明なコマンド: %s" },
    { KEY_USE_HELP, L"'qiin-tabtip help' を使用してヘルプを表示" }
  };

  // Initialize locale
  void init_locale(const std::string& locale) {
    current_locale = locale;
  }

  // Get current locale
  std::string get_current_locale() {
    return current_locale;
  }

  // Get localized string
  std::wstring get_localized_string(const std::string& key) {
    if (current_locale == "zh" || current_locale == "zh_CN" || current_locale == "zh_TW") {
      auto it = CHINESE_STRINGS.find(key);
      if (it != CHINESE_STRINGS.end()) {
        return it->second;
      }
    }

    if (current_locale == "ja" || current_locale == "ja_JP") {
      auto it = JAPANESE_STRINGS.find(key);
      if (it != JAPANESE_STRINGS.end()) {
        return it->second;
      }
    }

    // Fallback to English
    auto it = DEFAULT_STRINGS.find(key);
    if (it != DEFAULT_STRINGS.end()) {
      return it->second;
    }

    // Return key if not found
    std::wstring result(key.begin(), key.end());
    return result;
  }

  // Get localized string with format arguments
  std::wstring get_localized_string_fmt(const std::string& key, ...) {
    std::wstring format = get_localized_string(key);
    
    va_list args;
    va_start(args, key);
    
    wchar_t buffer[512];
    vswprintf_s(buffer, sizeof(buffer) / sizeof(wchar_t), format.c_str(), args);
    
    va_end(args);
    
    return std::wstring(buffer);
  }
} // namespace qiin_tabtip_i18n

