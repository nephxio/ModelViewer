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
		glfwTerminate();
	}

	void ModelViewerWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

	}
}