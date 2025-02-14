#pragma once

#include "ModelViewerWindow.h"
#include "ModelViewerSwapChain.h"
#include "glm/glm.hpp"

#include <memory>
#include <cassert>

namespace ModelViewer
{
	struct SimplePushConstantData
	{
		glm::mat2 transform{ 1.f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	class ModelViewerRenderer
	{
	public:
		ModelViewerRenderer(std::shared_ptr<ModelViewerWindow> window, std::shared_ptr<ModelViewerDevice> device);
		~ModelViewerRenderer();

		ModelViewerRenderer(const ModelViewerRenderer&) = delete;
		ModelViewerRenderer& operator=(const ModelViewerRenderer&) = delete;

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const 
		{ 
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress!");
			return commandBuffers[currentImageIndex]; 
		}

		VkRenderPass getSwapChainRenderPass() const { return modelViewerSwapChain->getRenderPass(); }

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		std::shared_ptr<ModelViewerWindow> modelViewerWindow;
		std::shared_ptr<ModelViewerDevice> modelViewerDevice;
		std::shared_ptr<ModelViewerSwapChain> modelViewerSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		bool isFrameStarted{ false };
	};
} // namespace ModelViewer