#include "ModelViewer.h"
#include "ModelViewerSimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <cassert>
#include <stdexcept>
#include <array>

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

		modelViewerWindow = std::make_shared<ModelViewerWindow>(WIDTH, HEIGHT, "Vulkan Window");
		modelViewerDevice = std::make_shared<ModelViewerDevice>(*modelViewerWindow);
		modelViewerRenderer = std::make_unique<ModelViewerRenderer>(modelViewerWindow, modelViewerDevice);

		loadModelObjects();

	}

	ModelViewer::~ModelViewer()
	{
	}

	void ModelViewer::loadModelObjects()
	{
		std::vector<ModelViewerModel::Vertex> vertices
		{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		auto modelViewerModel = std::make_shared<ModelViewerModel>(*modelViewerDevice, vertices);

		auto triangle = ModelViewerObject::createGameObject();
		triangle.model = modelViewerModel;
		triangle.color = { 0.1f, 0.8f, 0.1f };
		triangle.transform2d.translation.x = 0.2f;
		triangle.transform2d.scale = { 1.0f, 1.0f };
		triangle.transform2d.rotation = .25 * glm::two_pi<float>();

		modelObjects.push_back(std::move(triangle));
	}

	void ModelViewer::run()
	{
		ModelViewerSimpleRenderSystem simpleRenderSystem{ modelViewerDevice, modelViewerRenderer->getSwapChainRenderPass() };
		while (!modelViewerWindow->shouldClose())
		{
			glfwPollEvents();

			if (auto commandBuffer = modelViewerRenderer->beginFrame())
			{
				modelViewerRenderer->beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderModelObjects(commandBuffer, modelObjects);
				modelViewerRenderer->endSwapChainRenderPass(commandBuffer);
				modelViewerRenderer->endFrame();
			}
		}

		vkDeviceWaitIdle(modelViewerDevice->device());
	}
}
