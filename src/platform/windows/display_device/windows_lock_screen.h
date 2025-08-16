#pragma once

// the most stupid windows include (because it needs to be first...)
#include <windows.h>

// standard includes
#include <functional>
#include <memory>
#include <thread>

// local includes
#include "src/display_device/display_device.h"
namespace display_device::w_utils {
  /**
   * @brief Session event listener class for handling lock/unlock events.
   */
  class session_event_listener_t {
  public:
    /**
     * @brief Callback function type for session events.
     * @param is_locked True if session is locked, false if unlocked.
     */
    using session_state_callback_t = std::function<void(bool is_locked)>;

    /**
     * @brief Constructor.
     * @param callback Function to call when session state changes.
     */
    session_event_listener_t(session_state_callback_t callback);

    /**
     * @brief Destructor - automatically unregisters session notification.
     */
    ~session_event_listener_t();

    /**
     * @brief Start listening for session events.
     * @returns True if successfully started, false otherwise.
     */
    bool
    start();

    /**
     * @brief Stop listening for session events.
     */
    void
    stop();

    /**
     * @brief Check if the listener is currently active.
     * @returns True if listening for events, false otherwise.
     */
    bool
    is_active() const;

  private:
    session_state_callback_t callback_;
    HWND hwnd_;
    bool is_active_;
    std::thread message_thread_;
    
    /**
     * @brief Create a hidden window to receive WTS session notifications.
     */
    bool
    create_notification_window();

    /**
     * @brief Window procedure for handling WTS messages.
     */
    static LRESULT CALLBACK
    window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Message loop for the notification window.
     */
    void
    message_loop();

    /**
     * @brief Check current lock state during initialization.
     */
    void
    check_current_lock_state();
  };

  /**
   * @brief Initialize event-driven session monitoring.
   * @param callback Function to call when session state changes.
   * @returns Unique pointer to session listener, or nullptr on failure.
   */
  std::unique_ptr<session_event_listener_t>
  create_session_event_listener(session_event_listener_t::session_state_callback_t callback);
}  // namespace display_device::w_utils
