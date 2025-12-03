/**
 * @file src/system_tray_rust.cpp
 * @brief System tray implementation using the Rust tray library
 *
 * This file provides a thin C++ wrapper around the Rust tray library.
 * All menu logic, i18n, and event handling is done in Rust.
 */

#if defined(SUNSHINE_TRAY) && SUNSHINE_TRAY >= 1

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <tlhelp32.h>
  #define ICON_PATH_NORMAL WEB_DIR "images/sunshine.ico"
  #define ICON_PATH_PLAYING WEB_DIR "images/sunshine-playing.ico"
  #define ICON_PATH_PAUSING WEB_DIR "images/sunshine-pausing.ico"
  #define ICON_PATH_LOCKED WEB_DIR "images/sunshine-locked.ico"
#elif defined(__linux__) || defined(linux) || defined(__linux)
  #define ICON_PATH_NORMAL "sunshine-tray"
  #define ICON_PATH_PLAYING "sunshine-playing"
  #define ICON_PATH_PAUSING "sunshine-pausing"
  #define ICON_PATH_LOCKED "sunshine-locked"
#elif defined(__APPLE__) || defined(__MACH__)
  #define ICON_PATH_NORMAL WEB_DIR "images/logo-sunshine-16.png"
  #define ICON_PATH_PLAYING WEB_DIR "images/sunshine-playing-16.png"
  #define ICON_PATH_PAUSING WEB_DIR "images/sunshine-pausing-16.png"
  #define ICON_PATH_LOCKED WEB_DIR "images/sunshine-locked-16.png"
#endif

// Boost includes
#include <boost/filesystem.hpp>

// Local includes
#include "config.h"
#include "confighttp.h"
#include "display_device/session.h"
#include "entry_handler.h"
#include "file_handler.h"
#include "logging.h"
#include "platform/common.h"
#include "system_tray.h"
#include "version.h"

// Rust tray API
#include "rust_tray/include/rust_tray.h"

using namespace std::literals;

namespace system_tray {

  static std::atomic<bool> tray_initialized = false;
  static std::thread tray_thread;
  static std::atomic<bool> tray_thread_running = false;
  static std::atomic<bool> tray_thread_should_exit = false;

  // Forward declarations
  static void handle_tray_action(uint32_t action);

  /**
   * @brief Handle tray actions from Rust
   */
  static void handle_tray_action(uint32_t action) {
    switch (action) {
      case TRAY_ACTION_OPEN_UI:
        BOOST_LOG(debug) << "Opening UI from system tray"sv;
        launch_ui();
        break;

      case TRAY_ACTION_TOGGLE_VDD_MONITOR:
        BOOST_LOG(info) << "Toggling display power from system tray"sv;
        display_device::session_t::get().toggle_display_power();
        // Disable toggle for 10 seconds
        tray_set_vdd_enabled(0);
        std::thread([]() {
          std::this_thread::sleep_for(10s);
          tray_set_vdd_enabled(1);
        }).detach();
        break;

      case TRAY_ACTION_IMPORT_CONFIG:
        BOOST_LOG(info) << "Import config requested"sv;
        // Config import is handled in Rust for file dialog
        break;

      case TRAY_ACTION_EXPORT_CONFIG:
        BOOST_LOG(info) << "Export config requested"sv;
        // Config export is handled in Rust for file dialog
        break;

      case TRAY_ACTION_RESET_CONFIG:
        BOOST_LOG(info) << "Reset config requested"sv;
        // Reset config handled in C++ for now
        break;

      case TRAY_ACTION_LANGUAGE_CHINESE:
      case TRAY_ACTION_LANGUAGE_ENGLISH:
      case TRAY_ACTION_LANGUAGE_JAPANESE: {
        std::string locale;
        switch (action) {
          case TRAY_ACTION_LANGUAGE_CHINESE: locale = "zh"; break;
          case TRAY_ACTION_LANGUAGE_ENGLISH: locale = "en"; break;
          case TRAY_ACTION_LANGUAGE_JAPANESE: locale = "ja"; break;
        }

        // Save to config file
        try {
          auto vars = config::parse_config(file_handler::read_file(config::sunshine.config_file.c_str()));
          std::stringstream configStream;
          vars["tray_locale"] = locale;
          for (const auto& [key, value] : vars) {
            if (!value.empty() && value != "null") {
              configStream << key << " = " << value << std::endl;
            }
          }
          file_handler::write_file(config::sunshine.config_file.c_str(), configStream.str());
          BOOST_LOG(info) << "Tray language setting saved"sv;
        } catch (const std::exception& e) {
          BOOST_LOG(warning) << "Failed to save tray language: " << e.what();
        }
        break;
      }

      case TRAY_ACTION_STAR_PROJECT:
        // Handled in Rust (opens URL)
        BOOST_LOG(debug) << "Star project clicked"sv;
        break;

      case TRAY_ACTION_DONATE_YUNDI339:
      case TRAY_ACTION_DONATE_QIIN:
        // Handled in Rust (opens URL)
        BOOST_LOG(debug) << "Donation link clicked"sv;
        break;

      case TRAY_ACTION_RESET_DISPLAY_DEVICE_CONFIG:
        BOOST_LOG(info) << "Resetting display device config"sv;
        display_device::session_t::get().reset_persistence();
        break;

      case TRAY_ACTION_RESTART:
        BOOST_LOG(info) << "Restarting from system tray"sv;
        platf::restart();
        break;

      case TRAY_ACTION_QUIT:
        BOOST_LOG(info) << "Quitting from system tray"sv;
#ifdef _WIN32
        terminate_gui_processes();
        if (GetConsoleWindow() == NULL) {
            lifetime::exit_sunshine(ERROR_SHUTDOWN_IN_PROGRESS, true);
        } else {
            lifetime::exit_sunshine(0, true);
        }
#else
        lifetime::exit_sunshine(0, true);
#endif
        break;

      default:
        BOOST_LOG(warning) << "Unknown tray action: " << action;
        break;
    }
  }

  void terminate_gui_processes() {
#ifdef _WIN32
    BOOST_LOG(info) << "Terminating sunshine-gui.exe processes..."sv;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
      PROCESSENTRY32W pe32;
      pe32.dwSize = sizeof(PROCESSENTRY32W);

      if (Process32FirstW(snapshot, &pe32)) {
        do {
          if (wcscmp(pe32.szExeFile, L"sunshine-gui.exe") == 0) {
            BOOST_LOG(info) << "Found sunshine-gui.exe (PID: " << pe32.th32ProcessID << "), terminating..."sv;
            HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if (process_handle != NULL) {
              if (TerminateProcess(process_handle, 0)) {
                BOOST_LOG(info) << "Successfully terminated sunshine-gui.exe"sv;
              }
              CloseHandle(process_handle);
            }
          }
        } while (Process32NextW(snapshot, &pe32));
      }
      CloseHandle(snapshot);
    }
#endif
  }

  int init_tray() {
    if (tray_initialized.exchange(true)) {
      BOOST_LOG(warning) << "Tray already initialized"sv;
      return 0;
    }

    // Get locale from config
    std::string locale = "zh";  // Default to Chinese
    try {
      auto vars = config::parse_config(file_handler::read_file(config::sunshine.config_file.c_str()));
      if (vars.count("tray_locale") > 0) {
        locale = vars["tray_locale"];
      }
    } catch (...) {
      // Ignore errors, use default locale
    }

    // Create tooltip with version
    std::string tooltip = "Sunshine "s + PROJECT_VER;

    // Initialize the Rust tray
    int result = tray_init_ex(
      ICON_PATH_NORMAL,
      ICON_PATH_PLAYING,
      ICON_PATH_PAUSING,
      ICON_PATH_LOCKED,
      tooltip.c_str(),
      locale.c_str(),
      handle_tray_action
    );

    if (result != 0) {
      BOOST_LOG(error) << "Failed to initialize Rust tray"sv;
      tray_initialized = false;
      return -1;
    }

    BOOST_LOG(info) << "Rust tray initialized successfully"sv;
    return 0;
  }

  int process_tray_events() {
    if (!tray_initialized) {
      return -1;
    }
    return tray_loop(0);  // Non-blocking
  }

  int end_tray() {
    if (!tray_initialized) {
      return 0;
    }

    tray_exit();
    tray_initialized = false;

    BOOST_LOG(info) << "Rust tray shut down"sv;
    return 0;
  }

  int init_tray_threaded() {
    if (tray_thread_running.exchange(true)) {
      BOOST_LOG(warning) << "Tray thread already running"sv;
      return 0;
    }

    tray_thread_should_exit = false;

    tray_thread = std::thread([]() {
      if (init_tray() != 0) {
        tray_thread_running = false;
        return;
      }

      while (!tray_thread_should_exit.load()) {
        if (tray_loop(1) < 0) {  // Blocking with timeout
          break;
        }
      }

      end_tray();
      tray_thread_running = false;
    });

    return 0;
  }

  void update_tray_playing(std::string app_name) {
    if (!tray_initialized) return;

    tray_set_icon(TRAY_ICON_TYPE_PLAYING);

    std::string tooltip = "Sunshine - Playing: " + app_name;
    tray_set_tooltip(tooltip.c_str());
  }

  void update_tray_pausing(std::string app_name) {
    if (!tray_initialized) return;

    tray_set_icon(TRAY_ICON_TYPE_PAUSING);

    std::string tooltip = "Sunshine - Paused: " + app_name;
    tray_set_tooltip(tooltip.c_str());
  }

  void update_tray_stopped(std::string app_name) {
    if (!tray_initialized) return;

    tray_set_icon(TRAY_ICON_TYPE_NORMAL);

    std::string tooltip = "Sunshine "s + PROJECT_VER;
    tray_set_tooltip(tooltip.c_str());
  }

  void update_tray_require_pin(std::string pin_name) {
    if (!tray_initialized) return;

    tray_show_notification(
      "Sunshine",
      ("PIN required for: " + pin_name).c_str(),
      TRAY_ICON_TYPE_NORMAL
    );
  }

  void update_tray_vmonitor_checked(int checked) {
    if (!tray_initialized) return;
    tray_set_vdd_checked(checked);
  }

  // Stub implementations for compatibility
  std::string get_localized_string(const std::string& key) {
    // Localization is handled in Rust
    return key;
  }

  std::wstring get_localized_wstring(const std::string& key) {
    std::string s = get_localized_string(key);
    return std::wstring(s.begin(), s.end());
  }

}  // namespace system_tray

#endif  // SUNSHINE_TRAY
