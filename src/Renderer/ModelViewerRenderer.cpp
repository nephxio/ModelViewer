#include "ModelViewerRenderer.h"
#include "ModelViewerModel.h"
#include "ModelViewerPipeline.h"

#include <array>

namespace ModelViewer
{
	ModelViewerRenderer::ModelViewerRenderer(std::shared_ptr<ModelViewerWindow> window, std::shared_ptr<ModelViewerDevice> device) :
		modelViewerWindow{ window }, modelViewerDevice { device }
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	ModelViewerRenderer::~ModelViewerRenderer()
	{
		freeCommandBuffers();
	}

	void ModelViewerRenderer::recreateSwapChain()
	{
		auto extent = modelViewerWindow->getExtent();

		while (extent.width == 0 || extent.height == 0)
		{
			extent = modelViewerWindow->getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(modelViewerDevice->device());

		if (modelViewerSwapChain == nullptr)
		{
			modelViewerSwapChain = std::make_shared<ModelViewerSwapChain>(*modelViewerDevice, extent);
		}
		else
		{
			std::shared_ptr<ModelViewerSwapChain> oldSwapChain = std::move(modelViewerSwapChain);
			modelViewerSwapChain = std::make_shared<ModelViewerSwapChain>(*modelViewerDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*modelViewerSwapChain))
			{
				throw std::runtime_error("Swap chain image or depth format has changed!");
			}
		}
	}

	void ModelViewerRenderer::createCommandBuffers()
	{
		commandBuffers.resize(ModelViewerSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = modelViewerDevice->getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(modelViewerDevice->device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate Command Buffers!");
		}
	}

	void ModelViewerRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(modelViewerDevice->device(), modelViewerDevice->getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer ModelViewerRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Can't call begin frame while already in progress!");

		auto result = modelViewerSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire next swap chain image!");
		}
			
		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording Command Buffer!");
		}

		return commandBuffer;
	}

	void ModelViewerRenderer::endFrame()
	{
		assert(isFrameStarted && "Can't call endFrame while frame is in progress!");

		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}

		auto result = modelViewerSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || modelViewerWindow->wasWindowResized())
		{
			modelViewerWindow->resetWindowResizedFlag();
			recreateSwapChain();
		}

		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % ModelViewerSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void ModelViewerRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert (isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress!");
		assert (commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = modelViewerSwapChain->getRenderPass();
		renderPassInfo.framebuffer = modelViewerSwapChain->getFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = modelViewerSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(modelViewerSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(modelViewerSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, modelViewerSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);



	}

	void ModelViewerRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert (isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress!");
		assert (commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame!");

		vkCmdEndRenderPass(commandBuffer);

	}
} // namespace ModelViewer