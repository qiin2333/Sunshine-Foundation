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
    // 强制显示窗口
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    // 设置窗口为最顶层
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    return true;
  }
  
  return false;
}

/**
 * 显示触摸键盘
 */
bool ShowKeyboard() {
  // 首先检查 TabTip.exe 是否存在
  if (!CheckTabTipExists()) {
    std::wcerr << L"错误: 找不到 TabTip.exe" << std::endl;
    std::wcerr << L"路径: " << TABTIP_PATH << std::endl;
    std::wcerr << L"请检查 Windows 版本是否支持触摸键盘" << std::endl;
    return false;
  }

  // 检查并启用桌面模式自动调用（Windows 10/11 必需）
  if (!IsDesktopModeAutoInvokeEnabled()) {
    std::wcout << L"正在启用桌面模式自动调用..." << std::endl;
    if (EnableDesktopModeAutoInvoke()) {
      std::wcout << L"✓ 已启用桌面模式自动调用" << std::endl;
    } else {
      std::wcerr << L"⚠ 警告: 无法修改注册表，键盘可能不会显示" << std::endl;
    }
  }

  // 首先检查窗口是否已存在但隐藏
  HWND existingWnd = FindWindow(L"IPTip_Main_Window", NULL);
  if (existingWnd == NULL) {
    existingWnd = FindWindow(L"ApplicationFrameWindow", L"Microsoft Text Input Application");
  }

  if (existingWnd != NULL) {
    std::wcout << L"检测到键盘窗口已存在，正在显示..." << std::endl;
    if (ForceShowKeyboardWindow()) {
      std::wcout << L"✓ 触摸键盘已显示" << std::endl;
      return true;
    }
  }

  // 如果窗口不存在，启动 TabTip.exe
  std::wcout << L"正在启动 TabTip.exe..." << std::endl;
  
  SHELLEXECUTEINFO sei = { 0 };
  sei.cbSize = sizeof(SHELLEXECUTEINFO);
  sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
  sei.lpVerb = L"open";
  sei.lpFile = TABTIP_PATH;
  sei.nShow = SW_SHOW;

  if (ShellExecuteEx(&sei)) {
    if (sei.hProcess) {
      // 等待进程启动
      WaitForSingleObject(sei.hProcess, 1000);
      CloseHandle(sei.hProcess);
    }
    
    // 等待窗口创建
    Sleep(500);
    
    // 尝试显示窗口
    if (ForceShowKeyboardWindow()) {
      std::wcout << L"✓ 触摸键盘已启动并显示" << std::endl;
      return true;
    }
    
    std::wcout << L"触摸键盘进程已启动" << std::endl;
    return true;
  }

  // 获取错误信息
  DWORD error = GetLastError();
  std::wcerr << L"ShellExecuteEx 失败，错误代码: " << error << std::endl;

  // 备用方法
  HINSTANCE result = ShellExecute(NULL, L"open", TABTIP_PATH, NULL, NULL, SW_SHOW);
  if ((INT_PTR)result > 32) {
    Sleep(500);
    ForceShowKeyboardWindow();
    std::wcout << L"触摸键盘已启动 (备用方法)" << std::endl;
    return true;
  }

  std::wcerr << L"无法启动触摸键盘，错误代码: " << (INT_PTR)result << std::endl;
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
 * 显示使用帮助
 */
void ShowHelp() {
  std::wcout << L"Windows 虚拟触摸键盘工具\n" << std::endl;
  std::wcout << L"用法:" << std::endl;
  std::wcout << L"  virtual-keyboard [选项]\n" << std::endl;
  std::wcout << L"选项:" << std::endl;
  std::wcout << L"  show      - 显示触摸键盘" << std::endl;
  std::wcout << L"  hide      - 隐藏触摸键盘" << std::endl;
  std::wcout << L"  toggle    - 切换键盘显示状态 (默认)" << std::endl;
  std::wcout << L"  status    - 检查键盘是否可见" << std::endl;
  std::wcout << L"  diagnose  - 诊断系统环境" << std::endl;
  std::wcout << L"  help      - 显示此帮助信息" << std::endl;
  std::wcout << L"\n示例:" << std::endl;
  std::wcout << L"  virtual-keyboard             # 切换键盘状态" << std::endl;
  std::wcout << L"  virtual-keyboard show        # 显示键盘" << std::endl;
  std::wcout << L"  virtual-keyboard hide        # 隐藏键盘" << std::endl;
  std::wcout << L"  virtual-keyboard diagnose    # 诊断问题" << std::endl;
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

