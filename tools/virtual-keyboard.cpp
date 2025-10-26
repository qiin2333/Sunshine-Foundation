/**
 * @file tools/virtual-keyboard.cpp
 * @brief 调出或隐藏 Windows 触摸虚拟键盘的工具
 */
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
 * 显示触摸键盘
 */
bool ShowKeyboard() {
  // 方法1: 通过启动 TabTip.exe
  SHELLEXECUTEINFO sei = { 0 };
  sei.cbSize = sizeof(SHELLEXECUTEINFO);
  sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
  sei.lpVerb = L"open";
  sei.lpFile = TABTIP_PATH;
  sei.nShow = SW_SHOW;

  if (ShellExecuteEx(&sei)) {
    if (sei.hProcess) {
      CloseHandle(sei.hProcess);
    }
    std::wcout << L"触摸键盘已启动" << std::endl;
    return true;
  }

  // 方法2: 如果方法1失败，尝试使用注册表方法触发
  HINSTANCE result = ShellExecute(NULL, L"open", TABTIP_PATH, NULL, NULL, SW_SHOW);
  if ((INT_PTR)result > 32) {
    std::wcout << L"触摸键盘已启动 (备用方法)" << std::endl;
    return true;
  }

  std::wcerr << L"无法启动触摸键盘" << std::endl;
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
 * 显示使用帮助
 */
void ShowHelp() {
  std::wcout << L"Windows 虚拟触摸键盘工具\n" << std::endl;
  std::wcout << L"用法:" << std::endl;
  std::wcout << L"  virtual-keyboard [选项]\n" << std::endl;
  std::wcout << L"选项:" << std::endl;
  std::wcout << L"  show    - 显示触摸键盘" << std::endl;
  std::wcout << L"  hide    - 隐藏触摸键盘" << std::endl;
  std::wcout << L"  toggle  - 切换键盘显示状态 (默认)" << std::endl;
  std::wcout << L"  status  - 检查键盘是否可见" << std::endl;
  std::wcout << L"  help    - 显示此帮助信息" << std::endl;
  std::wcout << L"\n示例:" << std::endl;
  std::wcout << L"  virtual-keyboard          # 切换键盘状态" << std::endl;
  std::wcout << L"  virtual-keyboard show     # 显示键盘" << std::endl;
  std::wcout << L"  virtual-keyboard hide     # 隐藏键盘" << std::endl;
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

