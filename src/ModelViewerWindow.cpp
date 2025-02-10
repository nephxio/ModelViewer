#include "ModelViewerWindow.h"

namespace ModelViewer
{
	ModelViewerWindow::ModelViewerWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name }
	{
		initWindow();
	}

	ModelViewerWindow::~ModelViewerWindow()
	{
		glfwDestroyWindow(window);
	}

	void ModelViewerWindow::frameBufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto modelViewerWindow = reinterpret_cast<ModelViewerWindow*>(glfwGetWindowUserPointer(window));
		modelViewerWindow->frameBufferResized = true;
		modelViewerWindow->width = width;
		modelViewerWindow->height = height;

	}

	void ModelViewerWindow::initWindow()
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	}

	void ModelViewerWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface!");
		}
	}
}