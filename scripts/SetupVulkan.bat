@echo off

REM Function to compare Vulkan SDK versions
call :compare_versions

REM Check if Vulkan SDK is installed
if exist "%VULKAN_SDK%" (
    set "version_file=%VULKAN_SDK%\VERSION.txt"
    if exist "%version_file%" (
        for /f "tokens=*" %%i in (%version_file%) do set "vulkan_version=%%i"
        echo Detected Vulkan SDK version: %vulkan_version%

        call :compare_versions %vulkan_version% 1.3.296.0
        if %errorlevel% lss 0 (
            echo Your Vulkan SDK version is lower than 1.3.296.0.
            set "install_prompt=true"
        ) else (
            echo Your Vulkan SDK version is 1.3.296.0 or greater.
            set "install_prompt=false"
        )
    ) else (
        echo VERSION.txt not found. Assuming Vulkan SDK is not installed.
        set "install_prompt=true"
    )
) else (
    echo Vulkan SDK is not installed.
    set "install_prompt=true"
)

REM Prompt the user for installation if needed
if "%install_prompt%"=="true" (
    set /p install=Do you want to install Vulkan SDK? (Y/N): 
    if /i "%install%" neq "Y" (
        echo Installation canceled.
        pause
        exit /b 0
    )

    REM Set the URL for Vulkan SDK installer (replace with the correct URL if needed)
    set "sdk_url=https://sdk.lunarg.com/sdk/download/latest/windows/vulkan_sdk.exe"

    REM Set the destination folder for the installer
    set "installer_path=%TEMP%\vulkan_sdk.exe"

    REM Download the Vulkan SDK installer
    curl -L -o "%installer_path%" "%sdk_url%"

    REM Check if the installer was downloaded successfully
    if not exist "%installer_path%" (
        echo Failed to download Vulkan SDK installer.
        pause
        exit /b 1
    )

    REM Prompt the user to run the installer manually
    echo The Vulkan SDK installer has been downloaded to %installer_path%.
    echo Please run the installer manually to complete the installation.
    pause

    REM Cleanup: Delete the installer after execution
    del "%installer_path%"
)

echo Vulkan SDK installation process is complete.
pause
exit /b 0

:compare_versions
setlocal EnableDelayedExpansion

set "version1=%1"
set "version2=%2"

for /f "tokens=1-4 delims=." %%a in ("%version1%") do (
    set "ver1_major=%%a"
    set "ver1_minor=%%b"
    set "ver1_patch=%%c"
    set "ver1_build=%%d"
)

for /f "tokens=1-4 delims=." %%a in ("%version2%") do (
    set "ver2_major=%%a"
    set "ver2_minor=%%b"
    set "ver2_patch=%%c"
    set "ver2_build=%%d"
)

if not defined ver1_build set "ver1_build=0"
if not defined ver2_build set "ver2_build=0"

if !ver1_major! lss !ver2_major! (exit /b -1)
if !ver1_major! gtr !ver2_major! (exit /b 1)

if !ver1_minor! lss !ver2_minor! (exit /b -1)
if !ver1_minor! gtr !ver2_minor! (exit /b 1)

if !ver1_patch! lss !ver2_patch! (exit /b -1)
if !ver1_patch! gtr !ver2_patch! (exit /b 1)

if !ver1_build! lss !ver2_build! (exit /b -1)
if !ver1_build! gtr !ver2_build! (exit /b 1)
pause
exit /b 0
