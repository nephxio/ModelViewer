@echo off
setlocal

set "VULKAN_VERSION=1.3.290"
set "INSTALLER_URL=https://sdk.lunarg.com/sdk/download/1.3.296.0/windows/VulkanSDK-1.3.296.0-Installer.exe"

rem Check if Vulkan SDK is installed
for /f "tokens=4 delims=: " %%i in ('vulkaninfo ^| find "Vulkan Instance Version:"') do set "INSTALLED_VERSION=%%i"

rem Debug line to print the current Vulkan version
echo Installed Vulkan SDK version: %INSTALLED_VERSION%

if "%INSTALLED_VERSION%" neq "%VULKAN_VERSION%" (
    echo Vulkan SDK version %VULKAN_VERSION% is not installed or is not the correct version.
    set /p "choice=Do you want to fetch and install Vulkan SDK version %VULKAN_VERSION%? (Y/N): "
    
    if /i "%choice%"=="Y" (
        echo Downloading Vulkan SDK installer...
        powershell -Command "Invoke-WebRequest -Uri %INSTALLER_URL% -OutFile VulkanSDK-Installer.exe"
        echo Running installer...
        start /wait VulkanSDK-Installer.exe
        echo Installation complete.
    ) else (
        echo Installation aborted.
    )
) else (
    echo Vulkan SDK version %VULKAN_VERSION% is already installed.
)

endlocal
pause