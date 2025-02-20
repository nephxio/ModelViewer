#pragma once

#include "ModelViewerWindow.h"
#include "ModelViewerSwapChain.h"
#include "glm/glm.hpp"

#include <memory>
#include <cassert>

namespace ModelViewer
{
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
		std::shared_ptr<ModelViewerSwapChain> getSwapChain() { return modelViewerSwapChain; }

		VkCommandBuffer getCurrentCommandBuffer() const 
		{ 
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress!");
			return commandBuffers[currentFrameIndex]; 
		}

		int getFrameIndex() const
		{
			assert(isFrameStarted && "Cannot get frame index when frame not in progress!");
			return currentFrameIndex;
		}

		VkRenderPass getSwapChainRenderPass() const { return modelViewerSwapChain->getRenderPass(); }
		float getAspectRatio() const { return modelViewerSwapChain->extentAspectRatio(); }

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		std::shared_ptr<ModelViewerWindow> modelViewerWindow;
		std::shared_ptr<ModelViewerDevice> modelViewerDevice;
		std::shared_ptr<ModelViewerSwapChain> modelViewerSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
} // namespace ModelViewer