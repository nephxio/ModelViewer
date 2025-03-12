#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <string>
#include <stdexcept>

namespace ModelViewer
{
	class ModelViewerWindow
	{

	public:
		ModelViewerWindow(int w, int h, std::string name);
		~ModelViewerWindow();

		ModelViewerWindow(const ModelViewerWindow&) = delete;
		ModelViewerWindow& operator=(const ModelViewerWindow&) = delete;


		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		int getWidth() { return (uint32_t)width; }
		int getHeight() { return (uint32_t)height; }
		bool wasWindowResized() { return frameBufferResized; }
		void resetWindowResizedFlag() { frameBufferResized = false; }
		GLFWwindow* getGLFWWindow() const { return window; }


		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	
	private:
		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		std::string windowName;

		int width;
		int height;
		bool frameBufferResized;

		GLFWwindow* window;
	};
}