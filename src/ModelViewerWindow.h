#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <string>

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
	
	private:
		void initWindow();

		std::string windowName;

		const int width;
		const int height;

		GLFWwindow* window;
	};
}