#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "ModelViewer.h"

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
		createPipelineLayout();
		createPipeline();
	}

	ModelViewer::~ModelViewer()
	{
		vkDestroyPipelineLayout(modelViewerDevice->device(), pipelineLayout, nullptr);
	}

	void ModelViewer::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(modelViewerDevice->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Pipeline Layout!");
		}
	}

	void ModelViewer::createPipeline()
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		ModelViewerPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = modelViewerRenderer->getSwapChainRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		modelViewerPipeline = std::make_unique<ModelViewerPipeline>(*modelViewerDevice,
			"..\\src\\shaders\\simple_shader.vert.spv",
			"..\\src\\shaders\\simple_shader.frag.spv",
			pipelineConfig);

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

	void ModelViewer::renderModelObjects(VkCommandBuffer commandBuffer)
	{
		modelViewerPipeline->bind(commandBuffer);

		for (auto& object : modelObjects)
		{
			object.transform2d.rotation = glm::mod(object.transform2d.rotation + .01f, glm::two_pi<float>());
			SimplePushConstantData push{};
			push.offset = object.transform2d.translation;
			push.color = object.color;
			push.transform = object.transform2d.mat2();

			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			object.model->bind(commandBuffer);
			object.model->draw(commandBuffer);
		}
	}

	void ModelViewer::run()
	{
		while (!modelViewerWindow->shouldClose())
		{
			glfwPollEvents();

			if (auto commandBuffer = modelViewerRenderer->beginFrame())
			{
				modelViewerRenderer->beginSwapChainRenderPass(commandBuffer);
				renderModelObjects(commandBuffer);
				modelViewerRenderer->endSwapChainRenderPass(commandBuffer);
				modelViewerRenderer->endFrame();
			}
		}

		vkDeviceWaitIdle(modelViewerDevice->device());
	}
}
