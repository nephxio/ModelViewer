@echo off

REM Set the URLs for the Premake download and the license file
set "premake_url=https://github.com/premake/premake-core/releases/download/v5.0.0-beta4/premake-5.0.0-beta4-windows.zip"
set "license_url=https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"

REM Change directory to the folder containing the script
cd /d "%~dp0"

REM Navigate to the parent directory
cd ..

REM Set the destination folder
set "dest_folder=external/premake/bin"

REM Output the current directory and destination folder for debugging
echo Current Directory: %cd%
echo Destination Folder: %dest_folder%

REM Check if premake.exe already exists
if exist "%dest_folder%/premake.exe" (
    echo Premake executable already exists. Skipping download.
    pause
    exit /b 0
)

REM Create the destination folder if it doesn't exist
if not exist "%dest_folder%" mkdir "%dest_folder%"

REM Check if the destination folder was created successfully
if not exist "%dest_folder%" (
    echo Failed to create destination folder: %dest_folder%
    pause
    exit /b 1
)

REM Download the Premake zip file
curl -L -o "%dest_folder%/premake.zip" "%premake_url%"

REM Check if the download was successful
if not exist "%dest_folder%/premake.zip" (
    echo Failed to download Premake zip file.
    pause
    exit /b 1
)

REM Unzip the Premake file with the -Force parameter
powershell -Command "Expand-Archive -Path '%dest_folder%/premake.zip' -DestinationPath '%dest_folder%' -Force"

REM Check if the unzip operation was successful
if exist "%dest_folder%/premake.zip" (
    del "%dest_folder%/premake.zip"
) else (
    echo Failed to unzip Premake file.
    pause
    exit /b 1
)

REM Download the license file
curl -L -o "%dest_folder%/LICENSE.txt" "%license_url%"

REM Check if the license file was downloaded successfully
if exist "%dest_folder%/LICENSE.txt" (
    echo License file downloaded successfully.
) else (
    echo Failed to download license file.
    pause
    exit /b 1
)

echo Premake and the license file have been downloaded and extracted to %dest_folder%
pause
