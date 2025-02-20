workspace "ModelViewer"
    architecture "x64"
    startproject "ModelViewer"
    configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLM"] = "external/glm"
IncludeDir["GLFW"] = "external/glfw/include"
IncludeDir["ImGui"] = "external/imgui"
IncludeDir["VulkanSDK"] = os.getenv("VULKAN_SDK") .. "/Include"

LibDir = {}
LibDir["VulkanSDK"] = os.getenv("VULKAN_SDK") .. "/Lib"

print("GLM Include Path: " .. IncludeDir["GLM"])

group "Dependencies"
    include "external/glfw"
    include "external/imgui"
group ""

project "ModelViewer"
    location "src"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "src",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.VulkanSDK}"
    }

    libdirs
    {
        "%{LibDir.VulkanSDK}",
        "bin/" .. outputdir .. "/GLFW",
		"bin/" .. outputdir .. "/ImGui"
    }

    links
    {
        "GLFW",
        "ImGui",
        "vulkan-1"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "PLATFORM_WINDOWS" }
        linkoptions { "/NODEFAULTLIB:LIBCMTD" }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"
        linkoptions { "/NODEFAULTLIB:LIBCMTD" }

    filter "configurations:Release"
        defines "NDEBUG"
        runtime "Release"
        optimize "on"
        linkoptions { "/NODEFAULTLIB:LIBCMT" }
