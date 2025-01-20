#include "ModelViewer.h"

namespace ModelViewer
{
	ModelViewer::ModelViewer()
	{
		primaryMonitor = glfwGetPrimaryMonitor();
		if (!primaryMonitor)
		{
			std::cerr << "Failed to get primary monitor!" << std::endl;
		}

		mode = glfwGetVideoMode(primaryMonitor);
		if (!mode)
		{
			std::cerr << "Failed to get video mode!" << std::endl;
		}

		WIDTH = static_cast<int>(mode->width * 0.75);
		HEIGHT = static_cast<int>(mode->height * 0.75);

		// Initialize the modelViewerWindow and modelViewerPipeline with the correct dimensions 
		modelViewerWindow = std::make_unique<ModelViewerWindow>(WIDTH, HEIGHT, "Vulkan Window"); 
		modelViewerPipeline = std::make_unique<ModelViewerPipeline>("..\\src\\shaders\\simple_shader.vert.spv", "..\\src\\shaders\\simple_shader.frag.spv");
	}

	void ModelViewer::run()
	{
		while (!modelViewerWindow->shouldClose())
		{
			glfwPollEvents();
		}
	}
}
