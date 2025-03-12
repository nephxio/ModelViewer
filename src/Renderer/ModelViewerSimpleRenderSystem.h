#pragma once

#include "Camera/ModelViewerCamera.h"
#include "ModelViewerDevice.h"
#include "ModelViewerPipeline.h"
#include "ModelViewerObject.h"

#include <memory>
#include <vector>

namespace ModelViewer
{
	struct SimplePushConstantData
	{
		glm::mat4 viewProjection{ 1.f };
		glm::mat4 transform{ 1.f };
	};

	class ModelViewerSimpleRenderSystem
	{
	public:
		ModelViewerSimpleRenderSystem(std::shared_ptr<ModelViewerDevice> device, VkRenderPass renderPass);
		~ModelViewerSimpleRenderSystem();

		ModelViewerSimpleRenderSystem(const ModelViewerSimpleRenderSystem&) = delete;
		ModelViewerSimpleRenderSystem& operator=(const ModelViewerSimpleRenderSystem&) = delete;

		void renderModelObjects(VkCommandBuffer commandBuffer, std::vector<ModelViewerObject>& modelObjects, const ModelViewerCamera& camera);
	private:
;
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		std::shared_ptr<ModelViewerDevice> modelViewerDevice;
		std::unique_ptr<ModelViewerPipeline> modelViewerPipeline;
		VkPipelineLayout pipelineLayout;
	};
}