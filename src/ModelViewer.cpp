#include "ModelViewer.h"

namespace ModelViewer
{
	ModelViewer::ModelViewer()
	{
		primaryMonitor = glfwGetPrimaryMonitor();
		if (!primaryMonitor)
		{
			throw std::runtime_error("Failed to get primary monitor!");
		}

		mode = glfwGetVideoMode(primaryMonitor);
		if (!mode)
		{
			throw std::runtime_error("Failed to get video mode!");
		}

		WIDTH = static_cast<int>(mode->width * 0.75);
		HEIGHT = static_cast<int>(mode->height * 0.75);

		// Initialize the modelViewerWindow and modelViewerPipeline with the correct dimensions 
		modelViewerWindow = std::make_unique<ModelViewerWindow>(WIDTH, HEIGHT, "Vulkan Window"); 
		modelViewerDevice = std::make_unique<ModelViewerDevice>(*modelViewerWindow);
		modelViewerPipeline = std::make_unique<ModelViewerPipeline>(*modelViewerDevice, 
			"..\\src\\shaders\\simple_shader.vert.spv", 
			"..\\src\\shaders\\simple_shader.frag.spv",
			ModelViewerPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT));
	}

	void ModelViewer::run()
	{
		while (!modelViewerWindow->shouldClose())
		{
			glfwPollEvents();
		}
	}
}
