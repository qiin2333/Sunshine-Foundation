/**
 * @file tools/qiin-tabtip.cpp
 * @brief Tool to show or hide Windows touch virtual keyboard
 * @note Optimized version - does not use C++ standard library to reduce file size
 */
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <initguid.h>
#include <Objbase.h>
#include "qiin-tabtip_i18n.h"

// Simple console output functions (replacement for iostream)
static void Print(const wchar_t* msg) {
  DWORD written;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  WriteConsoleW(hConsole, msg, lstrlenW(msg), &written, NULL);
  WriteConsoleW(hConsole, L"\r\n", 2, &written, NULL);
}

static void PrintError(const wchar_t* msg) {
  DWORD written;
  HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
  WriteConsoleW(hConsole, msg, lstrlenW(msg), &written, NULL);
  WriteConsoleW(hConsole, L"\r\n", 2, &written, NULL);
}

// 字符串比较（不区分大小写）
static bool StrEqualI(const wchar_t* str1, const wchar_t* str2) {
  return lstrcmpiW(str1, str2) == 0;
}

// ITipInvocation COM 接口 - 这是微软官方的触摸键盘 API
// CLSID for UIHostNoLaunch
DEFINE_GUID(CLSID_UIHostNoLaunch,
    0x4CE576FA, 0x83DC, 0x4f88, 0x95, 0x1C, 0x9D, 0x07, 0x82, 0xB4, 0xE3, 0x76);

// IID for ITipInvocation
DEFINE_GUID(IID_ITipInvocation,
    0x37c994e7, 0x432b, 0x4834, 0xa2, 0xf7, 0xdc, 0xe1, 0xf1, 0x3b, 0x83, 0x4b);

// ITipInvocation 接口定义
struct ITipInvocation : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Toggle(HWND wnd) = 0;
};

// Windows 10/11 触摸键盘路径
const wchar_t* TABTIP_PATH = L"C:\\Program Files\\Common Files\\microsoft shared\\ink\\TabTip.exe";

/**
 * 检查触摸键盘是否正在运行
 */
bool IsKeyboardVisible() {
  HWND hwnd = FindWindow(L"IPTip_Main_Window", NULL);
  if (hwnd == NULL) {
    // Windows 11 可能使用不同的类名
    hwnd = FindWindow(L"ApplicationFrameWindow", L"Microsoft Text Input Application");
  }
  
  if (hwnd != NULL) {
    // 检查窗口是否可见
    return IsWindowVisible(hwnd);
  }
  return false;
}

/**
 * 检查 TabTip.exe 是否存在
 */
bool CheckTabTipExists() {
  DWORD dwAttrib = GetFileAttributes(TABTIP_PATH);
  return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

/**
 * 启用触摸键盘的桌面模式自动调用
 * 这是 Windows 10/11 中显示 TabTip 的关键设置
 */
bool EnableDesktopModeAutoInvoke() {
  HKEY hKey;
  LONG result = RegOpenKeyEx(
    HKEY_CURRENT_USER,
    L"SOFTWARE\\Microsoft\\TabletTip\\1.7",
    0,
    KEY_READ | KEY_WRITE,
    &hKey
  );

  if (result != ERROR_SUCCESS) {
    // 如果键不存在，尝试创建
    result = RegCreateKeyEx(
      HKEY_CURRENT_USER,
      L"SOFTWARE\\Microsoft\\TabletTip\\1.7",
      0,
      NULL,
      REG_OPTION_NON_VOLATILE,
      KEY_READ | KEY_WRITE,
      NULL,
      &hKey,
      NULL
    );
    
    if (result != ERROR_SUCCESS) {
      return false;
    }
  }

  // 设置 EnableDesktopModeAutoInvoke 为 1
  DWORD value = 1;
  result = RegSetValueEx(
    hKey,
    L"EnableDesktopModeAutoInvoke",
    0,
    REG_DWORD,
    (BYTE*)&value,
    sizeof(DWORD)
  );

  RegCloseKey(hKey);
  return result == ERROR_SUCCESS;
}

/**
 * 检查是否已启用桌面模式自动调用
 */
bool IsDesktopModeAutoInvokeEnabled() {
  HKEY hKey;
  LONG result = RegOpenKeyEx(
    HKEY_CURRENT_USER,
    L"SOFTWARE\\Microsoft\\TabletTip\\1.7",
    0,
    KEY_READ,
    &hKey
  );

  if (result != ERROR_SUCCESS) {
    return false;
  }

  DWORD value = 0;
  DWORD size = sizeof(DWORD);
  result = RegQueryValueEx(
    hKey,
    L"EnableDesktopModeAutoInvoke",
    NULL,
    NULL,
    (BYTE*)&value,
    &size
  );

  RegCloseKey(hKey);
  return (result == ERROR_SUCCESS && value == 1);
}

/**
 * 强制显示已存在的键盘窗口
 */
bool ForceShowKeyboardWindow() {
  // 查找键盘窗口
  HWND hwnd = FindWindow(L"IPTip_Main_Window", NULL);
  
  if (hwnd == NULL) {
    // Windows 11 可能使用不同的类名
    hwnd = FindWindow(L"ApplicationFrameWindow", L"Microsoft Text Input Application");
  }

  if (hwnd != NULL) {
    // 显示窗口
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    
    // 确保窗口位置在屏幕内
    RECT rect;
    GetWindowRect(hwnd, &rect);
    int keyboardHeight = rect.bottom - rect.top;
    
    if (keyboardHeight > 0) {
      int screenWidth = GetSystemMetrics(SM_CXSCREEN);
      int screenHeight = GetSystemMetrics(SM_CYSCREEN);
      int keyboardWidth = rect.right - rect.left;
      int x = (screenWidth - keyboardWidth) / 2;
      int y = screenHeight - keyboardHeight - 50;
      
      SetWindowPos(hwnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
      Sleep(50);
      SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
    
    return IsWindowVisible(hwnd);
  }
  
  return false;
}

/**
 * 使用 COM 接口显示触摸键盘（推荐方法）
 * 这是 Microsoft 官方的 ITipInvocation 接口
 */
bool ShowKeyboardViaCOM() {
  HRESULT hr = CoInitialize(NULL);
  bool needsUninit = SUCCEEDED(hr);
  
  ITipInvocation* pTipInvocation = NULL;
  hr = CoCreateInstance(
    CLSID_UIHostNoLaunch,
    NULL,
    CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER,
    IID_ITipInvocation,
    (void**)&pTipInvocation
  );
  
  bool success = false;
  if (SUCCEEDED(hr) && pTipInvocation) {
    hr = pTipInvocation->Toggle(GetDesktopWindow());
    success = SUCCEEDED(hr);
    pTipInvocation->Release();
  }
  
  if (needsUninit) {
    CoUninitialize();
  }
  
  return success;
}

/**
 * 显示触摸键盘（综合方法）
 */
bool ShowKeyboard() {
  // Method 1: Use COM interface (most reliable method)
  if (ShowKeyboardViaCOM()) {
    Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_TOUCH_KEYBOARD_DISPLAYED).c_str());
    return true;
  }
  
  // Method 2: Traditional method as fallback
  if (!CheckTabTipExists()) {
    PrintError(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_TABTIP_NOT_FOUND).c_str());
    return false;
  }

  // 确保注册表设置正确
  if (!IsDesktopModeAutoInvokeEnabled()) {
    EnableDesktopModeAutoInvoke();
  }

  // 检查窗口是否已存在
  HWND existingWnd = FindWindow(L"IPTip_Main_Window", NULL);
  if (existingWnd == NULL) {
    existingWnd = FindWindow(L"ApplicationFrameWindow", L"Microsoft Text Input Application");
  }

  if (existingWnd != NULL) {
    if (ForceShowKeyboardWindow()) {
      Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_TOUCH_KEYBOARD_DISPLAYED).c_str());
      return true;
    }
  }

  // Launch TabTip.exe
  SHELLEXECUTEINFO sei = { 0 };
  sei.cbSize = sizeof(SHELLEXECUTEINFO);
  sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
  sei.lpVerb = L"open";
  sei.lpFile = TABTIP_PATH;
  sei.nShow = SW_SHOW;

  if (ShellExecuteEx(&sei)) {
    if (sei.hProcess) {
      WaitForSingleObject(sei.hProcess, 1000);
      CloseHandle(sei.hProcess);
    }
    Sleep(500);
    
    if (ForceShowKeyboardWindow()) {
      Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_TOUCH_KEYBOARD_DISPLAYED).c_str());
      return true;
    }
  }

  // Final fallback: OSK
  HINSTANCE result = ShellExecute(NULL, L"open", L"osk.exe", NULL, NULL, SW_SHOW);
  if ((INT_PTR)result > 32) {
    Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_ONSCREEN_KEYBOARD_DISPLAYED).c_str());
    return true;
  }
  
  PrintError(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_CANNOT_DISPLAY_KEYBOARD).c_str());
  return false;
}

/**
 * 隐藏触摸键盘
 */
bool HideKeyboard() {
  // 查找键盘窗口
  HWND hwnd = FindWindow(L"IPTip_Main_Window", NULL);
  if (hwnd == NULL) {
    // Windows 11 可能使用不同的类名
    hwnd = FindWindow(L"ApplicationFrameWindow", L"Microsoft Text Input Application");
  }

  if (hwnd != NULL && IsWindowVisible(hwnd)) {
    PostMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
    Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_TOUCH_KEYBOARD_HIDDEN).c_str());
    return true;
  }
  
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_KEYBOARD_NOT_RUNNING).c_str());
  return false;
}

/**
 * 切换触摸键盘状态
 */
bool ToggleKeyboard() {
  if (IsKeyboardVisible()) {
    return HideKeyboard();
  } else {
    return ShowKeyboard();
  }
}

/**
 * 诊断系统环境
 */
void Diagnose() {
  
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_SYSTEM_DIAGNOSTIC_INFO).c_str());
  Print(L"");
  
  // Check Windows version
  OSVERSIONINFOEX osvi = { 0 };
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  #if defined(_MSC_VER)
  #pragma warning(push)
  #pragma warning(disable: 4996)
  #endif
  GetVersionEx((LPOSVERSIONINFO)&osvi);
  #if defined(_MSC_VER)
  #pragma warning(pop)
  #endif
  Print(qiin_tabtip_i18n::get_localized_string_fmt(qiin_tabtip_i18n::KEY_WINDOWS_VERSION, osvi.dwMajorVersion, osvi.dwMinorVersion).c_str());
  
  // Check TabTip.exe
  Print(L"");
  Print(qiin_tabtip_i18n::get_localized_string_fmt(qiin_tabtip_i18n::KEY_TABTIP_PATH, TABTIP_PATH).c_str());
  Print(CheckTabTipExists() ? qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_TABTIP_EXISTS).c_str() : qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_TABTIP_NOT_EXISTS).c_str());
  
  // Check registry settings
  Print(L"");
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_REGISTRY_SETTINGS).c_str());
  std::wstring enableMsg = L"  EnableDesktopModeAutoInvoke: ";
  enableMsg += IsDesktopModeAutoInvokeEnabled() ? 
      L"✓ " + qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_ENABLED) : 
      L"✗ " + qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_DISABLED);
  Print(enableMsg.c_str());
  
  // Check keyboard window
  Print(L"");
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_CHECKING_KEYBOARD_WINDOW).c_str());
  HWND hwnd = FindWindow(L"IPTip_Main_Window", NULL);
  std::wstring visibilityLabel = L"  " + qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_VISIBILITY) + L": ";
  
  if (hwnd) {
    std::wstring msg = L"  IPTip_Main_Window: ✓ " + qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_FOUND_WIN10);
    Print(msg.c_str());
    msg = visibilityLabel + (IsWindowVisible(hwnd) ? qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_VISIBLE) : qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_HIDDEN));
    Print(msg.c_str());
  } else {
    std::wstring msg = L"  IPTip_Main_Window: ✗ " + qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_NOT_FOUND);
    Print(msg.c_str());
  }
  
  hwnd = FindWindow(L"ApplicationFrameWindow", L"Microsoft Text Input Application");
  if (hwnd) {
    std::wstring msg = L"  ApplicationFrameWindow: ✓ " + qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_FOUND_WIN11);
    Print(msg.c_str());
    msg = visibilityLabel + (IsWindowVisible(hwnd) ? qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_VISIBLE) : qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_HIDDEN));
    Print(msg.c_str());
  } else {
    std::wstring msg = L"  ApplicationFrameWindow: ✗ " + qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_NOT_FOUND);
    Print(msg.c_str());
  }
  
  Print(L"");
  std::wstring statusMsg = qiin_tabtip_i18n::get_localized_string(IsKeyboardVisible() ? qiin_tabtip_i18n::KEY_STATUS_VISIBLE : qiin_tabtip_i18n::KEY_STATUS_HIDDEN);
  Print(statusMsg.c_str());
}

/**
 * 显示屏幕键盘 (OSK)
 */
bool ShowOSK() {
  HINSTANCE result = ShellExecute(NULL, L"open", L"osk.exe", NULL, NULL, SW_SHOW);
  if ((INT_PTR)result > 32) {
    Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_ONSCREEN_KEYBOARD_DISPLAYED).c_str());
    return true;
  }
  PrintError(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_CANNOT_DISPLAY_OSK).c_str());
  return false;
}

/**
 * 显示使用帮助
 */
void ShowHelp() {
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_TOOL_TITLE).c_str());
  Print(L"");
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_USAGE).c_str());
  Print(L"  qiin-tabtip [options]");
  Print(L"");
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_OPTIONS).c_str());
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_SHOW_DESC).c_str());
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_HIDE_DESC).c_str());
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_TOGGLE_DESC).c_str());
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_OSK_DESC).c_str());
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_STATUS_DESC).c_str());
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_DIAGNOSE_DESC).c_str());
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_HELP_DESC).c_str());
  Print(L"");
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_EXAMPLES).c_str());
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_EXAMPLE_TOGGLE).c_str());
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_EXAMPLE_SHOW).c_str());
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_EXAMPLE_OSK).c_str());
  Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_EXAMPLE_DIAGNOSE).c_str());
}

int wmain(int argc, wchar_t* argv[]) {
  // 初始化语言环境，根据系统语言自动选择
  LANGID langId = GetUserDefaultUILanguage();
  if (PRIMARYLANGID(langId) == LANG_CHINESE) {
    qiin_tabtip_i18n::init_locale("zh");
  } else if (PRIMARYLANGID(langId) == LANG_JAPANESE) {
    qiin_tabtip_i18n::init_locale("ja");
  } else {
    qiin_tabtip_i18n::init_locale("en");
  }

  const wchar_t* command = L"toggle";
  wchar_t cmdLower[256] = {0};
  
  if (argc > 1) {
    command = argv[1];
    // 转换为小写用于比较
    lstrcpynW(cmdLower, command, 255);
    CharLowerW(cmdLower);
    command = cmdLower;
  }

  if (StrEqualI(command, L"show")) {
    return ShowKeyboard() ? 0 : 1;
  }
  else if (StrEqualI(command, L"hide")) {
    return HideKeyboard() ? 0 : 1;
  }
  else if (StrEqualI(command, L"toggle")) {
    return ToggleKeyboard() ? 0 : 1;
  }
  else if (StrEqualI(command, L"osk")) {
    return ShowOSK() ? 0 : 1;
  }
  else if (StrEqualI(command, L"status")) {
    if (IsKeyboardVisible()) {
      Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_STATUS_VISIBLE).c_str());
      return 0;
    } else {
      Print(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_STATUS_HIDDEN).c_str());
      return 1;
    }
  }
  else if (StrEqualI(command, L"diagnose") || StrEqualI(command, L"diag")) {
    Diagnose();
    return 0;
  }
  else if (StrEqualI(command, L"help") || StrEqualI(command, L"--help") || 
           StrEqualI(command, L"-h") || StrEqualI(command, L"/?")) {
    ShowHelp();
    return 0;
  }
  else {
    PrintError(qiin_tabtip_i18n::get_localized_string_fmt(qiin_tabtip_i18n::KEY_UNKNOWN_COMMAND, command).c_str());
    PrintError(qiin_tabtip_i18n::get_localized_string(qiin_tabtip_i18n::KEY_USE_HELP).c_str());
    return 1;
  }

  return 0;
}

// 如果没有 wmain 支持，使用普通的 main 函数
#ifndef _UNICODE
int main(int argc, char* argv[]) {
  // 获取命令行参数的宽字符版本
  LPWSTR* szArglist;
  int nArgs;
  
  szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
  if (szArglist == NULL) {
    PrintError(L"CommandLineToArgvW failed");
    return 1;
  }
  
  int result = wmain(nArgs, szArglist);
  
  LocalFree(szArglist);
  return result;
}
#endif

