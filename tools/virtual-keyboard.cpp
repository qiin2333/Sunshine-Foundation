/**
 * @file tools/virtual-keyboard.cpp
 * @brief 调出或隐藏 Windows 触摸虚拟键盘的工具
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
#include <iostream>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <initguid.h>
#include <Objbase.h>

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
  HRESULT hr = CoInitialize(nullptr);
  bool needsUninit = SUCCEEDED(hr);
  
  ITipInvocation* pTipInvocation = nullptr;
  hr = CoCreateInstance(
    CLSID_UIHostNoLaunch,
    nullptr,
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
  // 方法 1: 使用 COM 接口（最可靠的方法）
  if (ShowKeyboardViaCOM()) {
    std::wcout << L"✓ 触摸键盘已显示" << std::endl;
    return true;
  }
  
  // 方法 2: 传统方法作为备选
  if (!CheckTabTipExists()) {
    std::wcerr << L"✗ 找不到 TabTip.exe" << std::endl;
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
      std::wcout << L"✓ 触摸键盘已显示" << std::endl;
      return true;
    }
  }

  // 启动 TabTip.exe
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
      std::wcout << L"✓ 触摸键盘已显示" << std::endl;
      return true;
    }
  }

  // 最后备选：OSK
  HINSTANCE result = ShellExecute(NULL, L"open", L"osk.exe", NULL, NULL, SW_SHOW);
  if ((INT_PTR)result > 32) {
    std::wcout << L"✓ 屏幕键盘已显示" << std::endl;
    return true;
  }
  
  std::wcerr << L"✗ 无法显示键盘" << std::endl;
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
    std::wcout << L"触摸键盘已关闭" << std::endl;
    return true;
  }
  
  std::wcout << L"触摸键盘未运行或已隐藏" << std::endl;
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
  std::wcout << L"=== 系统诊断信息 ===" << std::endl;
  
  // 检查 Windows 版本
  OSVERSIONINFOEX osvi = { 0 };
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  #pragma warning(push)
  #pragma warning(disable: 4996)
  GetVersionEx((LPOSVERSIONINFO)&osvi);
  #pragma warning(pop)
  std::wcout << L"Windows 版本: " << osvi.dwMajorVersion << L"." << osvi.dwMinorVersion << std::endl;
  
  // 检查 TabTip.exe
  std::wcout << L"\nTabTip 路径: " << TABTIP_PATH << std::endl;
  if (CheckTabTipExists()) {
    std::wcout << L"TabTip.exe: ✓ 存在" << std::endl;
  } else {
    std::wcout << L"TabTip.exe: ✗ 不存在" << std::endl;
  }
  
  // 检查注册表设置
  std::wcout << L"\n注册表设置:" << std::endl;
  std::wcout << L"  EnableDesktopModeAutoInvoke: " << (IsDesktopModeAutoInvokeEnabled() ? L"✓ 已启用" : L"✗ 未启用") << std::endl;
  
  // 检查键盘窗口
  std::wcout << L"\n检查键盘窗口:" << std::endl;
  HWND hwnd = FindWindow(L"IPTip_Main_Window", NULL);
  if (hwnd) {
    std::wcout << L"  IPTip_Main_Window: ✓ 找到 (Windows 10)" << std::endl;
    std::wcout << L"  可见性: " << (IsWindowVisible(hwnd) ? L"可见" : L"隐藏") << std::endl;
  } else {
    std::wcout << L"  IPTip_Main_Window: ✗ 未找到" << std::endl;
  }
  
  hwnd = FindWindow(L"ApplicationFrameWindow", L"Microsoft Text Input Application");
  if (hwnd) {
    std::wcout << L"  ApplicationFrameWindow: ✓ 找到 (Windows 11)" << std::endl;
    std::wcout << L"  可见性: " << (IsWindowVisible(hwnd) ? L"可见" : L"隐藏") << std::endl;
  } else {
    std::wcout << L"  ApplicationFrameWindow: ✗ 未找到" << std::endl;
  }
  
  std::wcout << L"\n当前键盘状态: " << (IsKeyboardVisible() ? L"可见" : L"隐藏") << std::endl;
}

/**
 * 显示屏幕键盘 (OSK)
 */
bool ShowOSK() {
  HINSTANCE result = ShellExecute(NULL, L"open", L"osk.exe", NULL, NULL, SW_SHOW);
  if ((INT_PTR)result > 32) {
    std::wcout << L"✓ 屏幕键盘已显示" << std::endl;
    return true;
  }
  std::wcerr << L"✗ 无法显示屏幕键盘" << std::endl;
  return false;
}

/**
 * 显示使用帮助
 */
void ShowHelp() {
  std::wcout << L"Windows 虚拟触摸键盘工具\n" << std::endl;
  std::wcout << L"用法:" << std::endl;
  std::wcout << L"  virtual-keyboard [选项]\n" << std::endl;
  std::wcout << L"选项:" << std::endl;
  std::wcout << L"  show      - 显示触摸键盘 (TabTip)" << std::endl;
  std::wcout << L"  hide      - 隐藏触摸键盘" << std::endl;
  std::wcout << L"  toggle    - 切换键盘显示状态 (默认)" << std::endl;
  std::wcout << L"  osk       - 显示屏幕键盘 (OSK - 更可靠)" << std::endl;
  std::wcout << L"  status    - 检查键盘是否可见" << std::endl;
  std::wcout << L"  diagnose  - 诊断系统环境" << std::endl;
  std::wcout << L"  help      - 显示此帮助信息" << std::endl;
  std::wcout << L"\n示例:" << std::endl;
  std::wcout << L"  virtual-keyboard             # 切换键盘状态" << std::endl;
  std::wcout << L"  virtual-keyboard show        # 显示触摸键盘" << std::endl;
  std::wcout << L"  virtual-keyboard osk         # 显示屏幕键盘 (推荐)" << std::endl;
  std::wcout << L"  virtual-keyboard hide        # 隐藏键盘" << std::endl;
  std::wcout << L"  virtual-keyboard diagnose    # 诊断问题" << std::endl;
  std::wcout << L"\n注意:" << std::endl;
  std::wcout << L"  如果触摸键盘无法显示，请尝试 'osk' 命令使用屏幕键盘" << std::endl;
}

int wmain(int argc, wchar_t* argv[]) {
  // 设置控制台输出为 UTF-16
  _setmode(_fileno(stdout), _O_U16TEXT);
  _setmode(_fileno(stderr), _O_U16TEXT);

  std::wstring command = L"toggle";
  
  if (argc > 1) {
    command = argv[1];
    // 转换为小写
    for (auto& c : command) {
      c = towlower(c);
    }
  }

  if (command == L"show") {
    return ShowKeyboard() ? 0 : 1;
  }
  else if (command == L"hide") {
    return HideKeyboard() ? 0 : 1;
  }
  else if (command == L"toggle") {
    return ToggleKeyboard() ? 0 : 1;
  }
  else if (command == L"osk") {
    return ShowOSK() ? 0 : 1;
  }
  else if (command == L"status") {
    if (IsKeyboardVisible()) {
      std::wcout << L"触摸键盘当前: 可见" << std::endl;
      return 0;
    } else {
      std::wcout << L"触摸键盘当前: 隐藏" << std::endl;
      return 1;
    }
  }
  else if (command == L"diagnose" || command == L"diag") {
    Diagnose();
    return 0;
  }
  else if (command == L"help" || command == L"--help" || command == L"-h" || command == L"/?") {
    ShowHelp();
    return 0;
  }
  else {
    std::wcerr << L"未知命令: " << command << std::endl;
    std::wcerr << L"使用 'virtual-keyboard help' 查看帮助" << std::endl;
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
    std::cerr << "CommandLineToArgvW failed" << std::endl;
    return 1;
  }
  
  int result = wmain(nArgs, szArglist);
  
  LocalFree(szArglist);
  return result;
}
#endif

