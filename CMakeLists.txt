cmake_minimum_required(VERSION 3.12)
project(ModelViewer)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories for GLM and GLFW
include_directories(${PROJECT_SOURCE_DIR}/glm ${PROJECT_SOURCE_DIR}/glfw/include)

# Add source files
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.h")

# Set dependencies directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/dependencies)

# Fetch GLFW and GLM with specific version tags
include(FetchContent)

# Set the dependencies directory for FetchContent
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/dependencies)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8 # specify a stable version tag
  SOURCE_DIR     ${FETCHCONTENT_BASE_DIR}/glfw-src
  BINARY_DIR     ${FETCHCONTENT_BASE_DIR}/glfw-build
)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8 # specify a stable version tag
  SOURCE_DIR     ${FETCHCONTENT_BASE_DIR}/glm-src
  BINARY_DIR     ${FETCHCONTENT_BASE_DIR}/glm-build
)

FetchContent_MakeAvailable(glfw glm)

# Check for Vulkan
find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
  message(FATAL_ERROR "Vulkan SDK not found")
endif()

# Add executable
add_executable(ModelViewer ${SOURCES})

# Link GLFW, GLM, and Vulkan
target_link_libraries(ModelViewer PRIVATE glfw glm Vulkan::Vulkan)

# Set the working directory to the project source directory
set_target_properties(ModelViewer PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

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

# Visual Studio specific output directories
set_target_properties(ModelViewer PROPERTIES
    MSVC_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    MSVC_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
    MSVC_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib/Debug"
    MSVC_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib/Release"
)

# Ensure dependencies are built
add_dependencies(ModelViewer glfw glm)
