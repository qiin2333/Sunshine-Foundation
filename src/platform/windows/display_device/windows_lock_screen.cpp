// lib includes
#include <boost/algorithm/string.hpp>
#include <boost/uuid/name_generator_sha1.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

// standard includes
#include <chrono>
#include <iostream>
#include <system_error>
#include <thread>

// local includes
#include "src/logging.h"
#include "src/platform/windows/misc.h"
#include "src/utility.h"
#include "windows_utils.h"
#include "windows_lock_screen.h"
// Windows includes after "windows.h"
#include <SetupApi.h>
#include <wtsapi32.h>

namespace display_device::w_utils {
  // Session event listener implementation
  constexpr const wchar_t* WINDOW_CLASS_NAME = L"SunshineSessionListener";

  session_event_listener_t::session_event_listener_t(session_state_callback_t callback)
      : callback_(std::move(callback)), hwnd_(nullptr), is_active_(false) {
  }

  session_event_listener_t::~session_event_listener_t() {
    stop();
  }

  bool
  session_event_listener_t::start() {
    if (is_active_) {
      return true;
    }

    // Start the message loop in a separate thread
    message_thread_ = std::thread(&session_event_listener_t::message_loop, this);

    // Wait for the window to be created
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    return is_active_;
  }

  void
  session_event_listener_t::stop() {
    if (!is_active_) {
      return;
    }

    is_active_ = false;

    if (hwnd_) {
      // Send a quit message to the message loop
      PostMessage(hwnd_, WM_QUIT, 0, 0);
    }

    if (message_thread_.joinable()) {
      message_thread_.join();
    }

    hwnd_ = nullptr;
  }

  bool
  session_event_listener_t::is_active() const {
    return is_active_;
  }

  bool
  session_event_listener_t::create_notification_window() {
    // Register window class
    WNDCLASSW wc = {};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = WINDOW_CLASS_NAME;

    static bool class_registered = false;
    if (!class_registered) {
      if (!RegisterClassW(&wc)) {
        BOOST_LOG(error) << "[ScreenLock] 注册窗口类失败: " << GetLastError();
        return false;
      }
      class_registered = true;
    }

    // Create hidden window
    hwnd_ = CreateWindowW(
        WINDOW_CLASS_NAME,
        L"SunshineSessionListener",
        0,
        0, 0, 0, 0,
        HWND_MESSAGE,  // Message-only window
        nullptr,
        GetModuleHandle(nullptr),
        this);

    if (!hwnd_) {
      BOOST_LOG(error) << "[ScreenLock] 创建窗口失败: " << GetLastError();
      return false;
    }

    // Register for session notifications
    if (!WTSRegisterSessionNotification(hwnd_, NOTIFY_FOR_THIS_SESSION)) {
      BOOST_LOG(error) << "[ScreenLock] 注册监听失败: " << GetLastError();
      DestroyWindow(hwnd_);
      hwnd_ = nullptr;
      return false;
    }

    BOOST_LOG(info) << "[ScreenLock] 监听器启动成功";
    return true;
  }

  void
  session_event_listener_t::check_current_lock_state() {
    // 检查当前的锁屏状态
    if (w_utils::is_user_session_locked()) {
      BOOST_LOG(info) << "[ScreenLock] 初始化时检测到系统处于锁屏状态";
      if (callback_) {
        callback_(true);
      }
    } else {
      BOOST_LOG(info) << "[ScreenLock] 初始化时系统未锁屏";
    }
  }

  LRESULT CALLBACK
  session_event_listener_t::window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Get the instance pointer from window data
    session_event_listener_t* instance = nullptr;
    
    if (msg == WM_NCCREATE) {
      // Store the instance pointer
      CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
      instance = static_cast<session_event_listener_t*>(cs->lpCreateParams);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(instance));
    } else {
      instance = reinterpret_cast<session_event_listener_t*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    switch (msg) {
      case WM_WTSSESSION_CHANGE:
        if (instance && instance->callback_) {
          switch (wParam) {
            case WTS_SESSION_LOCK:
              BOOST_LOG(info) << "[ScreenLock] 检测到锁屏";
              instance->callback_(true);
              break;
            case WTS_SESSION_UNLOCK:
              BOOST_LOG(info) << "[ScreenLock] 检测到解锁";
              instance->callback_(false);
              break;
            default:
              // Other session events we don't care about
              break;
          }
        }
        return 0;

      case WM_DESTROY:
        if (instance && instance->hwnd_) {
          WTSUnRegisterSessionNotification(instance->hwnd_);
          BOOST_LOG(info) << "[ScreenLock] 监听器停止";
        }
        PostQuitMessage(0);
        return 0;

      default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
  }

  void
  session_event_listener_t::message_loop() {
    if (!create_notification_window()) {
      return;
    }

    is_active_ = true;

    // 检查当前的锁屏状态
    check_current_lock_state();

    try {
      MSG msg;
      while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    } catch (const std::exception& e) {
      BOOST_LOG(error) << "[ScreenLock] 消息循环遇到错误: " << e.what();
    }

    is_active_ = false;
  }

  std::unique_ptr<session_event_listener_t>
  create_session_event_listener(session_event_listener_t::session_state_callback_t callback) {
    auto listener = std::make_unique<session_event_listener_t>(std::move(callback));
    
    if (!listener->start()) {
      BOOST_LOG(error) << "[ScreenLock] 启动监听器失败";
      return nullptr;
    }

    return listener;
  }

}  // namespace display_device::w_utils