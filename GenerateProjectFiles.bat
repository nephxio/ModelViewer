call ".\scripts\SetupPremake.bat"
call ".\scripts\SetupVulkan.bat"
call ".\external\premake\bin\premake5.exe" vs2022
call ".\src\shaders\compileshaders.bat"
pause