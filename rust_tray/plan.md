# 系统托盘替换方案

## 目标
将现有的基于 C 库 `tray` 的系统托盘实现替换为 Rust 的 `tray-icon` 库，以提高可维护性并减少跨平台适配代码。

## 总体方案
1. 在项目根目录下创建 Rust 子目录 `rust_tray`，编写静态库实现与原 `tray` 库完全兼容的 C API（`tray_init`、`tray_update`、`tray_loop`、`tray_exit`）。
2. 使用 `bindgen` 从原 `third-party/tray/src/tray.h` 生成 Rust 绑定，保证结构体布局一致。
3. 修改 CMake 构建系统，移除对原 `tray` 库的编译，改为构建并链接 Rust 静态库。
4. 保留 `third-party/tray` 子模块中的头文件，确保 C++ 代码 `#include "tray/src/tray.h"` 仍然有效。
5. 不修改 `src/system_tray.cpp` 的业务逻辑，仅替换底层库的实现。

## 详细步骤

### 1. 创建 Rust 项目
在 `rust_tray/` 下初始化 Cargo 库：

```
rust_tray/
├── Cargo.toml
├── build.rs
└── src/
    └── lib.rs
```

**Cargo.toml 内容：**

```toml
[package]
name = "sunshine_tray"
version = "0.1.0"
edition = "2021"

[lib]
name = "tray"
crate-type = ["staticlib"]

[dependencies]
tray-icon = { version = "0.10", default-features = false, features = ["tray"] }
anyhow = "1.0"
lazy_static = "1.4"
libc = "0.2"
log = "0.4"

[build-dependencies]
bindgen = "0.69"
```

### 2. 生成 FFI 绑定（build.rs）
利用 `bindgen` 解析原头文件，生成与 C 完全一致的 Rust 结构体定义。

```rust
// build.rs
use std::env;
use std::path::PathBuf;

fn main() {
    let bindings = bindgen::Builder::default()
        .header("third-party/tray/src/tray.h")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings");
}
```

### 3. 实现 C API（lib.rs）

主要任务：

- 全局状态管理（`OnceLock<Mutex<Option<TrayState>>>`），存储 `TrayIcon` 实例及菜单项到 C 菜单的映射。
- `tray_init`：根据传入的 `tray` 结构体创建托盘图标和菜单，注册回调（调用 C 回调）。
- `tray_update`：更新图标、工具提示、菜单文本、勾选状态等；若设置了 `notification_*` 字段，显示通知。
- `tray_loop`：启动平台事件循环（例如 GTK 的 `main` 或 Windows 消息循环），阻塞直到 `tray_exit` 被调用。
- `tray_exit`：退出事件循环，清理资源。

关键代码框架：

```rust
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_int, c_void};
use std::sync::{Mutex, OnceLock};
use tray_icon::{TrayIcon, TrayIconBuilder, Menu, MenuItem, MenuId, TrayIconEvent};
use anyhow::Result;

struct TrayState {
    icon: TrayIcon,
    menu_map: Vec<(MenuId, *const tray_menu)>, // 用于回调查找
    // 事件循环句柄（例如 gtk::Application 或 winit 事件循环）
    event_loop: Option<...>,
}

static TRAY_STATE: OnceLock<Mutex<Option<TrayState>>> = OnceLock::new();

#[no_mangle]
pub extern "C" fn tray_init(t: *mut tray) -> c_int {
    // 错误处理返回 -1，成功 0
    match unsafe { do_init(t) } {
        Ok(_) => 0,
        Err(_) => -1,
    }
}

unsafe fn do_init(t: *mut tray) -> Result<()> {
    // 构建菜单（递归）
    let (menu, menu_map) = build_menu((*t).menu)?;

    // 加载图标
    let icon = load_icon(CStr::from_ptr((*t).icon).to_str()?)?;

    let builder = TrayIconBuilder::new()
        .with_icon(icon)
        .with_tooltip(CStr::from_ptr((*t).tooltip).to_str()?)
        .with_menu(Box::new(menu));

    let tray_icon = builder.build()?;

    // 存储状态
    let state = TrayState {
        icon: tray_icon,
        menu_map,
        event_loop: None,
    };
    TRAY_STATE.get_or_init(|| Mutex::new(None))
        .lock()
        .unwrap()
        .replace(state);

    Ok(())
}

// 其他函数类似实现
```

菜单构建时需递归处理子菜单，并为每个 `MenuItem` 设置回调：当用户点击时，根据 `MenuId` 从 `menu_map` 中找到对应的 C `tray_menu` 指针，调用其 `cb` 字段（若存在）。

### 4. 修改 CMake 构建

编辑 `cmake/targets/common.cmake`，注释或删除对原 `tray` 库的引用，替换为自定义命令构建 Rust 库。

```cmake
# 禁用原 tray 库
# add_subdirectory(third-party/tray)

# 添加 Rust 托盘库构建
set(RUST_TRAY_SOURCE_DIR "${CMAKE_SOURCE_DIR}/rust_tray")
set(RUST_TARGET_DIR "${CMAKE_BINARY_DIR}/rust_tray")
set(RUST_OUTPUT_DEBUG "${RUST_TARGET_DIR}/debug/libtray.a")
# Release 构建可根据需要选择
add_custom_command(
    OUTPUT ${RUST_OUTPUT_DEBUG}
    COMMAND ${CMAKE_COMMAND} -E env CARGO_TARGET_DIR=${RUST_TARGET_DIR} cargo build --manifest-path ${RUST_TRAY_SOURCE_DIR}/Cargo.toml
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building Rust Tray library (debug)"
    VERBATIM
)
add_custom_target(rust_tray ALL DEPENDS ${RUST_OUTPUT_DEBUG})

# 链接静态库
target_link_libraries(sunshine PRIVATE ${RUST_OUTPUT_DEBUG})
target_include_directories(sunshine PRIVATE third-party/tray/src)
```

注意：根据实际构建类型（Debug/Release）调整 cargo 参数和输出路径，也可同时构建 Release 版本并通过 CMake 变量选择。

### 5. 验证与测试

编译 Sunshine，确保无链接错误。运行后验证：
- 系统托盘图标显示正常。
- 菜单点击功能正常（打开 UI、开关显示器、导入导出配置、语言切换等）。
- 通知正常显示（例如开始/暂停串流、配对请求）。
- 图标切换（播放、暂停、锁定）正常。

## 可能的问题及应对

- **回调中 C 字符串的生命周期**：`tray_menu.text` 在语言切换时会指向新的 `std::string` 内部数据，而 Rust 在 `tray_update` 时会重新拷贝字符串，因此安全。
- **事件循环集成**：不同平台事件循环实现方式不同，需确保 `tray_loop` 阻塞且能正确响应退出。可参考 `tray-icon` 示例中的事件循环代码（如 winit、gtk）。
- **Windows 系统权限问题**：原 `system_tray.cpp` 中的线程 DACL 修改和等待 Shell 的代码保留，不影响 Rust 库。
- **跨平台图标格式**：原代码已通过宏区分各平台图标路径/名称，直接传递给 `tray-icon` 即可，该库会自动处理。

## 后续工作

- 移除 `third-party/tray` 中除头文件外的源文件（可选，但保留子模块可方便获取头文件）。
- 完善 Rust 实现中的错误处理和日志记录。
- 如有必要，在 CI 中增加 Rust 工具链安装步骤。

本方案通过最小侵入式修改实现了核心功能替换，可显著降低维护成本并提高跨平台稳定性。