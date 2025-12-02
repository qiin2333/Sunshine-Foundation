/**
 * @file rust_tray.h
 * @brief C API for the Rust tray library
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Menu action identifiers (must match Rust MenuAction enum)
 */
typedef enum {
    TRAY_ACTION_OPEN_UI = 1,
    TRAY_ACTION_TOGGLE_VDD_MONITOR = 2,
    TRAY_ACTION_IMPORT_CONFIG = 3,
    TRAY_ACTION_EXPORT_CONFIG = 4,
    TRAY_ACTION_RESET_CONFIG = 5,
    TRAY_ACTION_LANGUAGE_CHINESE = 6,
    TRAY_ACTION_LANGUAGE_ENGLISH = 7,
    TRAY_ACTION_LANGUAGE_JAPANESE = 8,
    TRAY_ACTION_STAR_PROJECT = 9,
    TRAY_ACTION_DONATE_YUNDI339 = 10,
    TRAY_ACTION_DONATE_QIIN = 11,
    TRAY_ACTION_RESET_DISPLAY_DEVICE_CONFIG = 12,
    TRAY_ACTION_RESTART = 13,
    TRAY_ACTION_QUIT = 14,
    TRAY_ACTION_NOTIFICATION_CLICKED = 15,
} TrayAction;

/**
 * @brief Icon types for tray_set_icon
 */
typedef enum {
    TRAY_ICON_NORMAL = 0,
    TRAY_ICON_PLAYING = 1,
    TRAY_ICON_PAUSING = 2,
    TRAY_ICON_LOCKED = 3,
} TrayIconType;

/**
 * @brief Callback function type for menu actions
 * @param action The action identifier
 */
typedef void (*TrayActionCallback)(uint32_t action);

/**
 * @brief Initialize the tray with extended options
 * @param icon_normal Path to normal icon
 * @param icon_playing Path to playing icon
 * @param icon_pausing Path to pausing icon
 * @param icon_locked Path to locked icon
 * @param tooltip Tooltip text
 * @param locale Initial locale (e.g., "zh", "en", "ja")
 * @param callback Callback function for menu actions
 * @return 0 on success, -1 on error
 */
int tray_init_ex(
    const char* icon_normal,
    const char* icon_playing,
    const char* icon_pausing,
    const char* icon_locked,
    const char* tooltip,
    const char* locale,
    TrayActionCallback callback
);

/**
 * @brief Run one iteration of the event loop
 * @param blocking If non-zero, block until an event is available
 * @return 0 on success, -1 if exit was requested
 */
int tray_loop(int blocking);

/**
 * @brief Exit the tray event loop
 */
void tray_exit(void);

/**
 * @brief Set the tray icon
 * @param icon_type Icon type (0=normal, 1=playing, 2=pausing, 3=locked)
 */
void tray_set_icon(int icon_type);

/**
 * @brief Set the tray tooltip
 * @param tooltip Tooltip text
 */
void tray_set_tooltip(const char* tooltip);

/**
 * @brief Update the VDD monitor toggle checkbox state
 * @param checked Non-zero to check, zero to uncheck
 */
void tray_set_vdd_checked(int checked);

/**
 * @brief Set the VDD toggle menu item enabled state
 * @param enabled Non-zero to enable, zero to disable
 */
void tray_set_vdd_enabled(int enabled);

/**
 * @brief Set the current locale
 * @param locale Locale string (e.g., "zh", "en", "ja")
 */
void tray_set_locale(const char* locale);

/**
 * @brief Show a notification
 * @param title Notification title
 * @param text Notification text
 * @param icon_type Icon type for the notification
 */
void tray_show_notification(const char* title, const char* text, int icon_type);

#ifdef __cplusplus
}
#endif
