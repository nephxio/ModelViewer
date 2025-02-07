#include "ModelViewer.h"

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

		// Initialize the modelViewerWindow
		modelViewerWindow = std::make_unique<ModelViewerWindow>(WIDTH, HEIGHT, "Vulkan Window");
		modelViewerDevice = std::make_unique<ModelViewerDevice>(*modelViewerWindow);
		modelViewerSwapChain = std::make_unique<ModelViewerSwapChain>(*modelViewerDevice, modelViewerWindow->getExtent());

		loadModels();
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();

	}

	ModelViewer::~ModelViewer()
	{
		vkDestroyPipelineLayout(modelViewerDevice->device(), pipelineLayout, nullptr);
	}

	void ModelViewer::loadModels()
	{
		std::vector<ModelViewerModel::Vertex> vertices
		{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		modelViewerModel = std::make_unique<ModelViewerModel>(*modelViewerDevice, vertices);
	}

	void ModelViewer::run()
	{
		while (!modelViewerWindow->shouldClose())
		{
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(modelViewerDevice->device());
	}

	void ModelViewer::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(modelViewerDevice->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Pipeline Layout!");
		}
	}

	void ModelViewer::createPipeline()
	{
		auto pipelineConfig = ModelViewerPipeline::defaultPipelineConfigInfo(modelViewerSwapChain->width(), modelViewerSwapChain->height());
		pipelineConfig.renderPass = modelViewerSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		modelViewerPipeline = std::make_unique<ModelViewerPipeline>(*modelViewerDevice,
			"..\\src\\shaders\\simple_shader.vert.spv",
			"..\\src\\shaders\\simple_shader.frag.spv",
			pipelineConfig);

	}

	void ModelViewer::createCommandBuffers()
	{
		commandBuffers.resize(modelViewerSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = modelViewerDevice->getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(modelViewerDevice->device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate Command Buffers!");
		}

		for (int i = 0; i < commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			
			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to begin recording Command Buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = modelViewerSwapChain->getRenderPass();
			renderPassInfo.framebuffer = modelViewerSwapChain->getFrameBuffer(i);
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = modelViewerSwapChain->getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data(); 

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			modelViewerPipeline->bind(commandBuffers[i]);
			modelViewerModel->bind(commandBuffers[i]);
			modelViewerModel->draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to record command buffer!");
			}
		}
	}
	void ModelViewer::drawFrame()
	{
		uint32_t imageIndex;
		auto result = modelViewerSwapChain->acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire next swap chain image!");
		}

		result = modelViewerSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image!");
		}

	}
}
