/**
 * @file src/platform/windows/win_dark_mode.h
 * @brief Windows dark mode support for the entire process
 *
 * This module provides process-wide dark mode support for Windows 10 1809+ and Windows 11.
 * It handles the undocumented Windows APIs for enabling dark mode for menus, dialogs, and windows.
 *
 * Note: This should be called early in program initialization, before creating any windows or menus.
 */

#pragma once

#ifdef _WIN32

#include <windows.h>

namespace win_dark_mode {

  /**
   * @brief Enable dark mode support for the entire process
   *
   * This function enables dark mode support by calling SetPreferredAppMode (or AllowDarkModeForApp
   * on older Windows versions). It should be called once during application initialization,
   * before creating any windows or system tray icons.
   *
   * The function will:
   * - Load the necessary APIs from uxtheme.dll
   * - Set the process to allow dark mode (follows system setting)
   * - This affects all menus, dialogs, and windows created after this call
   */
  void enable_process_dark_mode();

  /**
   * @brief Apply dark mode to a specific window's title bar
   *
   * This function applies the immersive dark mode to a window's title bar using
   * DwmSetWindowAttribute with DWMWA_USE_IMMERSIVE_DARK_MODE.
   *
   * @param hwnd Window handle to apply dark mode to
   * @param enable Whether to enable (true) or disable (false) dark mode for the title bar
   */
  void apply_window_dark_title_bar(HWND hwnd, bool enable);

}  // namespace win_dark_mode

#endif  // _WIN32
