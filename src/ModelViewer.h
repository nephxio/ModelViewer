#pragma once

#include "ModelViewerDevice.h"
#include "ModelViewerWindow.h"
#include "ModelViewerPipeline.h"
#include "ModelViewerSwapChain.h"
#include "ModelViewerModel.h"

#include <iostream>
#include <memory>
#include <vector>

namespace ModelViewer
{
	class ModelViewer
	{
	public:
		ModelViewer();
		~ModelViewer();

		ModelViewer(const ModelViewer&) = delete;
		ModelViewer &operator=(const ModelViewer&) = delete;

		void run();
	private:
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		int WIDTH;
		int HEIGHT;

		GLFWmonitor* primaryMonitor;
		const GLFWvidmode* mode;
		std::unique_ptr<ModelViewerDevice> modelViewerDevice;
		std::unique_ptr<ModelViewerWindow> modelViewerWindow; 
		std::unique_ptr<ModelViewerPipeline> modelViewerPipeline;
		std::unique_ptr<ModelViewerSwapChain> modelViewerSwapChain;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<ModelViewerModel> modelViewerModel;
	};
}