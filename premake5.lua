workspace "ModelViewer"
    architecture "x64"
    startproject "ModelViewer"
    configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLM"] = "external/glm"
IncludeDir["GLFW"] = "external/glfw/include"
IncludeDir["ImGui"] = "external/imgui"

group "Dependencies"
    include "external/glfw"
    include "external/glm"
    include "external/imgui"
group ""

project "ModelViewer"
    location "ModelViewer"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.location}/src",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.ImGui}"
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

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "NDEBUG"
        runtime "Release"
        optimize "on"
