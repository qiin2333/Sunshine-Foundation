@echo off
REM 打包 SignPath 配置文件
REM 用于上传到 SignPath 控制台

setlocal

echo 正在打包 SignPath 配置文件...
echo.

REM 检查配置文件是否存在
if not exist "SignPath-installer.xml" (
    echo 错误: 找不到 SignPath-installer.xml 文件
    echo 请确保在项目根目录运行此脚本
    exit /b 1
)

if not exist "SignPath-portable.xml" (
    echo 错误: 找不到 SignPath-portable.xml 文件
    echo 请确保在项目根目录运行此脚本
    exit /b 1
)

REM 检查 7z 是否可用
where 7z >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 找不到 7z 命令
    echo 请安装 7-Zip: https://www.7-zip.org/
    exit /b 1
)

REM 删除旧的配置包
if exist "SignPath-installer-config.zip" (
    del "SignPath-installer-config.zip"
    echo 已删除旧的安装包配置
)

if exist "SignPath-portable-config.zip" (
    del "SignPath-portable-config.zip"
    echo 已删除旧的便携版配置
)

REM 打包配置文件
echo 正在创建配置包...
echo.
echo [1/2] 打包安装包配置...
7z a SignPath-installer-config.zip SignPath-installer.xml

if %errorlevel% neq 0 (
    echo ✗ 安装包配置打包失败
    exit /b 1
)

echo [2/2] 打包便携版配置...
7z a SignPath-portable-config.zip SignPath-portable.xml

if %errorlevel% neq 0 (
    echo ✗ 便携版配置打包失败
    exit /b 1
)

echo.
echo ✓ 所有配置文件打包成功！
echo.
echo 文件位置:
echo   - 安装包配置: %cd%\SignPath-installer-config.zip
echo   - 便携版配置: %cd%\SignPath-portable-config.zip
echo.
echo 下一步:
echo 1. 登录 SignPath 控制台: https://app.signpath.io
echo 2. 进入您的项目: sunshine-foundation
echo 3. 创建两个 Artifact Configurations:
echo    - windows-installer (上传 SignPath-installer-config.zip)
echo    - windows-portable (上传 SignPath-portable-config.zip)
echo.
