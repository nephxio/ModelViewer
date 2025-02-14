#pragma once

#include "ModelViewerDevice.h"
#include "ModelViewerWindow.h"
#include "ModelViewerPipeline.h"
#include "ModelViewerRenderer.h"
#include "ModelViewerSwapChain.h"
#include "ModelViewerModel.h"
#include "ModelViewerObject.h"

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

		void loadModelObjects();
		void createPipelineLayout();
		void createPipeline();
		void renderModelObjects(VkCommandBuffer commandBuffer);
		
		int WIDTH;
		int HEIGHT;

		GLFWmonitor* primaryMonitor;
		const GLFWvidmode* mode;

		std::shared_ptr<ModelViewerWindow> modelViewerWindow;
		std::shared_ptr<ModelViewerDevice> modelViewerDevice;
		std::unique_ptr<ModelViewerRenderer> modelViewerRenderer;
		std::unique_ptr<ModelViewerPipeline> modelViewerPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<ModelViewerObject> modelObjects;
	};
}