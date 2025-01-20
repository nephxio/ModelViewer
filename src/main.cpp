#include "ModelViewer.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return EXIT_FAILURE;
	}

	ModelViewer::ModelViewer modelViewer{};

	try
	{
		modelViewer.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}