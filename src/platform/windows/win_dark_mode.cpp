/**
 * @file src/platform/windows/win_dark_mode.cpp
 * @brief Implementation of Windows dark mode support
 */

#ifdef _WIN32

#include "win_dark_mode.h"

#include <dwmapi.h>
#include <mutex>

namespace win_dark_mode {

  // Undocumented PreferredAppMode enum from uxtheme.dll
  enum class PreferredAppMode {
    Default = 0,     // Use system default (usually light)
    AllowDark = 1,   // Allow dark mode (follow system setting)
    ForceDark = 2,   // Force dark mode regardless of system setting
    ForceLight = 3,  // Force light mode regardless of system setting
    Max = 4,
  };

  // Function pointer types for undocumented uxtheme.dll APIs
  // Windows 10 1903+ uses SetPreferredAppMode (ordinal 135)
  // Windows 10 1809-1903 uses AllowDarkModeForApp (ordinal 135) - same ordinal, different signature
  // We use SetPreferredAppMode as it works on all modern Windows versions
  using SetPreferredAppModeFn = PreferredAppMode(WINAPI *)(PreferredAppMode);

  // Global function pointer (initialized once)
  static SetPreferredAppModeFn g_SetPreferredAppMode = nullptr;
  static std::once_flag g_init_flag;

  /**
   * @brief Initialize the dark mode API function pointers
   *
   * This function loads uxtheme.dll and retrieves the undocumented function pointers.
   * It only runs once and caches the results using std::call_once for thread safety.
   */
  static void
  init_dark_mode_apis() {
    // Load uxtheme.dll
    HMODULE hUxTheme = LoadLibraryW(L"uxtheme.dll");
    if (!hUxTheme) {
      return;
    }

    // Get SetPreferredAppMode (ordinal 135)
    // This works on Windows 10 1809+ (build 17763+)
    // On older versions, the function will exist but may not have the expected effect
    g_SetPreferredAppMode =
      reinterpret_cast<SetPreferredAppModeFn>(
        GetProcAddress(hUxTheme, MAKEINTRESOURCEA(135)));

    // Note: We intentionally don't call FreeLibrary(hUxTheme) because we need
    // the function pointers to remain valid for the lifetime of the process
  }

  void
  enable_process_dark_mode() {
    // Thread-safe one-time initialization
    std::call_once(g_init_flag, init_dark_mode_apis);

    // Call the function if available
    if (g_SetPreferredAppMode) {
      // Use AllowDark to follow the system's dark/light mode preference
      g_SetPreferredAppMode(PreferredAppMode::AllowDark);
    }
    // If API is not available, dark mode is not supported on this Windows version
    // (Windows 10 < 1809 or earlier). We silently do nothing in this case.
  }

  void
  apply_window_dark_title_bar(HWND hwnd, bool enable) {
    if (!hwnd) {
      return;
    }

    // DWMWA_USE_IMMERSIVE_DARK_MODE is documented for Windows 11
    // but also works on Windows 10 20H1+
    constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20;

    BOOL useDark = enable ? TRUE : FALSE;
    DwmSetWindowAttribute(
      hwnd,
      DWMWA_USE_IMMERSIVE_DARK_MODE,
      &useDark,
      sizeof(useDark));
  }

}  // namespace win_dark_mode

#endif  // _WIN32
