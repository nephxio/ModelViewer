@echo off

set SHADER_DIR=%~dp0..\src\shaders

echo Compiling shaders...
"%VULKAN_SDK%\Bin\glslc.exe" %SHADER_DIR%\simple_shader.vert -o %SHADER_DIR%\simple_shader.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" %SHADER_DIR%\simple_shader.frag -o %SHADER_DIR%\simple_shader.frag.spv
echo Finished compiling shaders.
pause