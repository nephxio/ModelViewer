@echo off
SET VULKAN_SDK=%VULKAN_SDK%

IF NOT DEFINED VULKAN_SDK (
    echo Vulkan SDK environment variable is not set.
    echo Please download the Vulkan SDK from the following link:
    echo https://sdk.lunarg.com/sdk/download/1.3.296.0/windows/VulkanSDK-1.3.296.0-Installer.exe
    pause
    exit /b 1
) ELSE (
    echo Vulkan SDK is set to %VULKAN_SDK%
)
pause