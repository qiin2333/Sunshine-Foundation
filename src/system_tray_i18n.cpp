#include "system_tray_i18n.h"
#include "config.h"

#ifdef _WIN32
  #include <windows.h>
#endif

namespace system_tray_i18n {
  // String key constants
  const std::string KEY_QUIT_TITLE = "quit_title";
  const std::string KEY_QUIT_MESSAGE = "quit_message";
  
  // Menu item keys
  const std::string KEY_OPEN_SUNSHINE = "open_sunshine";
  const std::string KEY_VDD_MONITOR_TOGGLE = "vdd_monitor_toggle";
  const std::string KEY_CONFIGURATION = "configuration";
  const std::string KEY_IMPORT_CONFIG = "import_config";
  const std::string KEY_EXPORT_CONFIG = "export_config";
  const std::string KEY_RESET_TO_DEFAULT = "reset_to_default";
  const std::string KEY_LANGUAGE = "language";
  const std::string KEY_CHINESE = "chinese";
  const std::string KEY_ENGLISH = "english";
  const std::string KEY_JAPANESE = "japanese";
  const std::string KEY_STAR_PROJECT = "star_project";
  const std::string KEY_HELP_US = "help_us";
  const std::string KEY_DOCTOR = "doctor";
  const std::string KEY_QIIN = "qiin";
  const std::string KEY_RESET_DISPLAY_DEVICE_CONFIG = "reset_display_device_config";
  const std::string KEY_RESTART = "restart";
  const std::string KEY_QUIT = "quit";
  
  // Notification message keys
  const std::string KEY_STREAM_STARTED = "stream_started";
  const std::string KEY_STREAMING_STARTED_FOR = "streaming_started_for";
  const std::string KEY_STREAM_PAUSED = "stream_paused";
  const std::string KEY_STREAMING_PAUSED_FOR = "streaming_paused_for";
  const std::string KEY_APPLICATION_STOPPED = "application_stopped";
  const std::string KEY_APPLICATION_STOPPED_MSG = "application_stopped_msg";
  const std::string KEY_INCOMING_PAIRING_REQUEST = "incoming_pairing_request";
  const std::string KEY_CLICK_TO_COMPLETE_PAIRING = "click_to_complete_pairing";
  
  // MessageBox keys
  const std::string KEY_ERROR_TITLE = "error_title";
  const std::string KEY_ERROR_NO_USER_SESSION = "error_no_user_session";
  const std::string KEY_IMPORT_SUCCESS_TITLE = "import_success_title";
  const std::string KEY_IMPORT_SUCCESS_MSG = "import_success_msg";
  const std::string KEY_IMPORT_ERROR_TITLE = "import_error_title";
  const std::string KEY_IMPORT_ERROR_WRITE = "import_error_write";
  const std::string KEY_IMPORT_ERROR_READ = "import_error_read";
  const std::string KEY_IMPORT_ERROR_EXCEPTION = "import_error_exception";
  const std::string KEY_EXPORT_SUCCESS_TITLE = "export_success_title";
  const std::string KEY_EXPORT_SUCCESS_MSG = "export_success_msg";
  const std::string KEY_EXPORT_ERROR_TITLE = "export_error_title";
  const std::string KEY_EXPORT_ERROR_WRITE = "export_error_write";
  const std::string KEY_EXPORT_ERROR_NO_CONFIG = "export_error_no_config";
  const std::string KEY_EXPORT_ERROR_EXCEPTION = "export_error_exception";
  const std::string KEY_RESET_CONFIRM_TITLE = "reset_confirm_title";
  const std::string KEY_RESET_CONFIRM_MSG = "reset_confirm_msg";
  const std::string KEY_RESET_SUCCESS_TITLE = "reset_success_title";
  const std::string KEY_RESET_SUCCESS_MSG = "reset_success_msg";
  const std::string KEY_RESET_ERROR_TITLE = "reset_error_title";
  const std::string KEY_RESET_ERROR_MSG = "reset_error_msg";
  const std::string KEY_RESET_ERROR_EXCEPTION = "reset_error_exception";
  const std::string KEY_FILE_DIALOG_SELECT_IMPORT = "file_dialog_select_import";
  const std::string KEY_FILE_DIALOG_SAVE_EXPORT = "file_dialog_save_export";
  const std::string KEY_FILE_DIALOG_CONFIG_FILES = "file_dialog_config_files";
  const std::string KEY_FILE_DIALOG_ALL_FILES = "file_dialog_all_files";

  // Default English strings
  const std::map<std::string, std::string> DEFAULT_STRINGS = {
    { KEY_QUIT_TITLE, "Wait! Don't Leave Me! T_T" },
    { KEY_QUIT_MESSAGE, "Nooo! You can't just quit like that!\nAre you really REALLY sure you want to leave?\nI'll miss you... but okay, if you must...\n\n(This will also close the Sunshine GUI application.)" },
    { KEY_OPEN_SUNSHINE, "Open Sunshine" },
    { KEY_VDD_MONITOR_TOGGLE, "VDD Monitor Toggle" },
    { KEY_CONFIGURATION, "Configuration" },
    { KEY_IMPORT_CONFIG, "Import Config" },
    { KEY_EXPORT_CONFIG, "Export Config" },
    { KEY_RESET_TO_DEFAULT, "Reset to Default" },
    { KEY_LANGUAGE, "Language" },
    { KEY_CHINESE, "中文" },
    { KEY_ENGLISH, "English" },
    { KEY_JAPANESE, "日本語" },
    { KEY_STAR_PROJECT, "Star Project" },
    { KEY_HELP_US, "Sponsor Us" },
    { KEY_DOCTOR, "Doctor" },
    { KEY_QIIN, "Qiin" },
    { KEY_RESET_DISPLAY_DEVICE_CONFIG, "Reset Display Device Config" },
    { KEY_RESTART, "Restart" },
    { KEY_QUIT, "Quit" },
    { KEY_STREAM_STARTED, "Stream Started" },
    { KEY_STREAMING_STARTED_FOR, "Streaming started for %s" },
    { KEY_STREAM_PAUSED, "Stream Paused" },
    { KEY_STREAMING_PAUSED_FOR, "Streaming paused for %s" },
    { KEY_APPLICATION_STOPPED, "Application Stopped" },
    { KEY_APPLICATION_STOPPED_MSG, "Application %s successfully stopped" },
    { KEY_INCOMING_PAIRING_REQUEST, "Incoming Pairing Request From: %s" },
    { KEY_CLICK_TO_COMPLETE_PAIRING, "Click here to complete the pairing process" },
    { KEY_ERROR_TITLE, "Error" },
    { KEY_ERROR_NO_USER_SESSION, "Cannot open file dialog: No active user session found." },
    { KEY_IMPORT_SUCCESS_TITLE, "Import Success" },
    { KEY_IMPORT_SUCCESS_MSG, "Configuration imported successfully!\nPlease restart Sunshine to apply changes." },
    { KEY_IMPORT_ERROR_TITLE, "Import Error" },
    { KEY_IMPORT_ERROR_WRITE, "Failed to import configuration file." },
    { KEY_IMPORT_ERROR_READ, "Failed to read the selected configuration file." },
    { KEY_IMPORT_ERROR_EXCEPTION, "An error occurred while importing configuration." },
    { KEY_EXPORT_SUCCESS_TITLE, "Export Success" },
    { KEY_EXPORT_SUCCESS_MSG, "Configuration exported successfully!" },
    { KEY_EXPORT_ERROR_TITLE, "Export Error" },
    { KEY_EXPORT_ERROR_WRITE, "Failed to export configuration file." },
    { KEY_EXPORT_ERROR_NO_CONFIG, "No configuration found to export." },
    { KEY_EXPORT_ERROR_EXCEPTION, "An error occurred while exporting configuration." },
    { KEY_RESET_CONFIRM_TITLE, "Reset Configuration" },
    { KEY_RESET_CONFIRM_MSG, "This will reset all configuration to default values.\nThis action cannot be undone.\n\nDo you want to continue?" },
    { KEY_RESET_SUCCESS_TITLE, "Reset Success" },
    { KEY_RESET_SUCCESS_MSG, "Configuration has been reset to default values.\nPlease restart Sunshine to apply changes." },
    { KEY_RESET_ERROR_TITLE, "Reset Error" },
    { KEY_RESET_ERROR_MSG, "Failed to reset configuration file." },
    { KEY_RESET_ERROR_EXCEPTION, "An error occurred while resetting configuration." },
    { KEY_FILE_DIALOG_SELECT_IMPORT, "Select Configuration File to Import" },
    { KEY_FILE_DIALOG_SAVE_EXPORT, "Save Configuration File As" },
    { KEY_FILE_DIALOG_CONFIG_FILES, "Configuration Files" },
    { KEY_FILE_DIALOG_ALL_FILES, "All Files" }
  };

  // Chinese strings
  const std::map<std::string, std::string> CHINESE_STRINGS = {
    { KEY_QUIT_TITLE, "真的要退出吗" },
    { KEY_QUIT_MESSAGE, "你不能退出!\n那么想退吗? 真拿你没办法呢, 继续点一下吧~\n\n这将同时关闭Sunshine GUI应用程序。" },
    { KEY_OPEN_SUNSHINE, "打开 Sunshine" },
    { KEY_VDD_MONITOR_TOGGLE, "虚拟显示器开关" },
    { KEY_CONFIGURATION, "配置" },
    { KEY_IMPORT_CONFIG, "导入配置" },
    { KEY_EXPORT_CONFIG, "导出配置" },
    { KEY_RESET_TO_DEFAULT, "重置为默认" },
    { KEY_LANGUAGE, "语言" },
    { KEY_CHINESE, "中文" },
    { KEY_ENGLISH, "English" },
    { KEY_JAPANESE, "日本語" },
    { KEY_STAR_PROJECT, "Star项目" },
    { KEY_HELP_US, "赞助我们" },
    { KEY_DOCTOR, "Doctor" },
    { KEY_QIIN, "Qiin" },
    { KEY_RESET_DISPLAY_DEVICE_CONFIG, "重置显示设备配置" },
    { KEY_RESTART, "重启" },
    { KEY_QUIT, "退出" },
    { KEY_STREAM_STARTED, "串流已开始" },
    { KEY_STREAMING_STARTED_FOR, "已开始串流 %s" },
    { KEY_STREAM_PAUSED, "串流已暂停" },
    { KEY_STREAMING_PAUSED_FOR, "已暂停串流 %s" },
    { KEY_APPLICATION_STOPPED, "应用已停止" },
    { KEY_APPLICATION_STOPPED_MSG, "应用 %s 已成功停止" },
    { KEY_INCOMING_PAIRING_REQUEST, "来自的配对请求: %s" },
    { KEY_CLICK_TO_COMPLETE_PAIRING, "点击此处完成配对过程" },
    { KEY_ERROR_TITLE, "错误" },
    { KEY_ERROR_NO_USER_SESSION, "无法打开文件对话框: 未找到活动的用户会话。" },
    { KEY_IMPORT_SUCCESS_TITLE, "导入成功" },
    { KEY_IMPORT_SUCCESS_MSG, "配置导入成功！\n请重启 Sunshine 以应用更改。" },
    { KEY_IMPORT_ERROR_TITLE, "导入错误" },
    { KEY_IMPORT_ERROR_WRITE, "导入配置文件失败。" },
    { KEY_IMPORT_ERROR_READ, "读取所选配置文件失败。" },
    { KEY_IMPORT_ERROR_EXCEPTION, "导入配置时发生错误。" },
    { KEY_EXPORT_SUCCESS_TITLE, "导出成功" },
    { KEY_EXPORT_SUCCESS_MSG, "配置导出成功！" },
    { KEY_EXPORT_ERROR_TITLE, "导出错误" },
    { KEY_EXPORT_ERROR_WRITE, "导出配置文件失败。" },
    { KEY_EXPORT_ERROR_NO_CONFIG, "未找到可导出的配置。" },
    { KEY_EXPORT_ERROR_EXCEPTION, "导出配置时发生错误。" },
    { KEY_RESET_CONFIRM_TITLE, "重置配置" },
    { KEY_RESET_CONFIRM_MSG, "这将把所有配置重置为默认值。\n此操作无法撤销。\n\n是否继续？" },
    { KEY_RESET_SUCCESS_TITLE, "重置成功" },
    { KEY_RESET_SUCCESS_MSG, "配置已重置为默认值。\n请重启 Sunshine 以应用更改。" },
    { KEY_RESET_ERROR_TITLE, "重置错误" },
    { KEY_RESET_ERROR_MSG, "重置配置文件失败。" },
    { KEY_RESET_ERROR_EXCEPTION, "重置配置时发生错误。" },
    { KEY_FILE_DIALOG_SELECT_IMPORT, "选择要导入的配置文件" },
    { KEY_FILE_DIALOG_SAVE_EXPORT, "保存配置文件为" },
    { KEY_FILE_DIALOG_CONFIG_FILES, "配置文件" },
    { KEY_FILE_DIALOG_ALL_FILES, "所有文件" }
  };

  const std::map<std::string, std::string> JAPANESE_STRINGS = {
    { KEY_QUIT_TITLE, "本当に終了しますか？" },
    { KEY_QUIT_MESSAGE, "終了できません！\n本当に終了したいですか？\n\nこれによりSunshine GUIアプリケーションも閉じられます。" },
    { KEY_OPEN_SUNSHINE, "Sunshineを開く" },
    { KEY_VDD_MONITOR_TOGGLE, "VDDモニタートグル" },
    { KEY_CONFIGURATION, "設定" },
    { KEY_IMPORT_CONFIG, "設定をインポート" },
    { KEY_EXPORT_CONFIG, "設定をエクスポート" },
    { KEY_RESET_TO_DEFAULT, "デフォルトにリセット" },
    { KEY_LANGUAGE, "言語" },
    { KEY_CHINESE, "中文" },
    { KEY_ENGLISH, "English" },
    { KEY_JAPANESE, "日本語" },
    { KEY_STAR_PROJECT, "プロジェクトにスター" },
    { KEY_HELP_US, "スポンサー" },
    { KEY_DOCTOR, "Doctor" },
    { KEY_QIIN, "Qiin" },
    { KEY_RESET_DISPLAY_DEVICE_CONFIG, "ディスプレイデバイス設定をリセット" },
    { KEY_RESTART, "再起動" },
    { KEY_QUIT, "終了" },
    { KEY_STREAM_STARTED, "ストリーム開始" },
    { KEY_STREAMING_STARTED_FOR, "%s のストリーミングを開始しました" },
    { KEY_STREAM_PAUSED, "ストリーム一時停止" },
    { KEY_STREAMING_PAUSED_FOR, "%s のストリーミングを一時停止しました" },
    { KEY_APPLICATION_STOPPED, "アプリケーション停止" },
    { KEY_APPLICATION_STOPPED_MSG, "アプリケーション %s が正常に停止しました" },
    { KEY_INCOMING_PAIRING_REQUEST, "ペアリング要求: %s" },
    { KEY_CLICK_TO_COMPLETE_PAIRING, "クリックしてペアリングプロセスを完了してください" },
    { KEY_ERROR_TITLE, "エラー" },
    { KEY_ERROR_NO_USER_SESSION, "ファイルダイアログを開けません: アクティブなユーザーセッションが見つかりません。" },
    { KEY_IMPORT_SUCCESS_TITLE, "インポート成功" },
    { KEY_IMPORT_SUCCESS_MSG, "設定のインポートに成功しました！\n変更を適用するにはSunshineを再起動してください。" },
    { KEY_IMPORT_ERROR_TITLE, "インポートエラー" },
    { KEY_IMPORT_ERROR_WRITE, "設定ファイルのインポートに失敗しました。" },
    { KEY_IMPORT_ERROR_READ, "選択した設定ファイルの読み取りに失敗しました。" },
    { KEY_IMPORT_ERROR_EXCEPTION, "設定のインポート中にエラーが発生しました。" },
    { KEY_EXPORT_SUCCESS_TITLE, "エクスポート成功" },
    { KEY_EXPORT_SUCCESS_MSG, "設定のエクスポートに成功しました！" },
    { KEY_EXPORT_ERROR_TITLE, "エクスポートエラー" },
    { KEY_EXPORT_ERROR_WRITE, "設定ファイルのエクスポートに失敗しました。" },
    { KEY_EXPORT_ERROR_NO_CONFIG, "エクスポートする設定が見つかりません。" },
    { KEY_EXPORT_ERROR_EXCEPTION, "設定のエクスポート中にエラーが発生しました。" },
    { KEY_RESET_CONFIRM_TITLE, "設定をリセット" },
    { KEY_RESET_CONFIRM_MSG, "これにより、すべての設定がデフォルト値にリセットされます。\nこの操作は元に戻せません。\n\n続行しますか？" },
    { KEY_RESET_SUCCESS_TITLE, "リセット成功" },
    { KEY_RESET_SUCCESS_MSG, "設定がデフォルト値にリセットされました。\n変更を適用するにはSunshineを再起動してください。" },
    { KEY_RESET_ERROR_TITLE, "リセットエラー" },
    { KEY_RESET_ERROR_MSG, "設定ファイルのリセットに失敗しました。" },
    { KEY_RESET_ERROR_EXCEPTION, "設定のリセット中にエラーが発生しました。" },
    { KEY_FILE_DIALOG_SELECT_IMPORT, "インポートする設定ファイルを選択" },
    { KEY_FILE_DIALOG_SAVE_EXPORT, "設定ファイルを保存" },
    { KEY_FILE_DIALOG_CONFIG_FILES, "設定ファイル" },
    { KEY_FILE_DIALOG_ALL_FILES, "すべてのファイル" }
  };

  // Get current locale from config
  std::string
  get_current_locale() {
    // Try to get from config::sunshine.tray_locale
    try {
      // Check if config is available
      if (!config::sunshine.tray_locale.empty()) {
        return config::sunshine.tray_locale;
      }
    }
    catch (...) {
      // If config is not available, fall back to default
    }

    // Default to English
    return "en";
  }

  // Set tray locale
  void
  set_tray_locale(const std::string &locale) {
    // Update config
    config::sunshine.tray_locale = locale;
  }

  // Get localized string
  std::string
  get_localized_string(const std::string &key) {
    std::string locale = get_current_locale();

    if (locale == "zh" || locale == "zh_CN" || locale == "zh_TW") {
      auto it = CHINESE_STRINGS.find(key);
      if (it != CHINESE_STRINGS.end()) {
        return it->second;
      }
    }

    if (locale == "ja" || locale == "ja_JP") {
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

    return key;  // Return key if not found
  }

  // Convert UTF-8 string to wide string
  std::wstring
  utf8_to_wstring(const std::string &utf8_str) {
    // Modern C++ approach: use Windows API on Windows, simple conversion on other platforms
  #ifdef _WIN32
    if (utf8_str.empty()) {
      return L"";
    }
    
    // Get required buffer size
    int wide_size = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
    if (wide_size == 0) {
      // Fallback: simple char-by-char conversion
      std::wstring result;
      result.reserve(utf8_str.length());
      for (char c : utf8_str) {
        result += static_cast<wchar_t>(c);
      }
      return result;
    }
    
    // Convert to wide string
    std::wstring result(wide_size - 1, L'\0');
    if (MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &result[0], wide_size) == 0) {
      // Fallback: simple char-by-char conversion
      result.clear();
      result.reserve(utf8_str.length());
      for (char c : utf8_str) {
        result += static_cast<wchar_t>(c);
      }
    }
    return result;
  #else
    // On non-Windows platforms, use simple char-by-char conversion
    // This is not perfect for UTF-8, but it's a reasonable fallback
    std::wstring result;
    result.reserve(utf8_str.length());
    for (char c : utf8_str) {
      result += static_cast<wchar_t>(c);
    }
    return result;
  #endif
  }
}  // namespace system_tray_i18n
