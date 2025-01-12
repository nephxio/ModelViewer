cmake_minimum_required(VERSION 3.12)
project(ModelViewer)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories for GLM, GLFW, and ImGui
include_directories(${PROJECT_SOURCE_DIR}/glm ${PROJECT_SOURCE_DIR}/glfw/include ${PROJECT_SOURCE_DIR}/imgui)

# Add source files excluding ImGui sources
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.h")

# Fetch GLFW, GLM, and ImGui with specific version tags
include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        master
)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        master
)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(glfw glm imgui)

# Check for Vulkan
find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
  message(FATAL_ERROR "Vulkan SDK not found")
endif()

# Add ImGui source files explicitly
file(GLOB IMGUI_SOURCES
  ${FETCHCONTENT_BASE_DIR}/imgui-src/*.cpp
  ${FETCHCONTENT_BASE_DIR}/imgui-src/backends/imgui_impl_glfw.cpp
  ${FETCHCONTENT_BASE_DIR}/imgui-src/backends/imgui_impl_vulkan.cpp
)

# Create the imgui target
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${FETCHCONTENT_BASE_DIR}/imgui-src ${FETCHCONTENT_BASE_DIR}/imgui-src/backends)
target_link_libraries(imgui PRIVATE glfw Vulkan::Vulkan)

# Add executable
add_executable(ModelViewer ${SOURCES})

# Link GLFW, GLM, ImGui, and Vulkan
target_link_libraries(ModelViewer PRIVATE glfw glm imgui Vulkan::Vulkan)

# Set binary output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set output directories for different configurations
set_target_properties(ModelViewer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib/Debug"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib/Release"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib/Debug"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib/Release"
)

# Ensure dependencies are built
add_dependencies(ModelViewer glfw glm imgui)
