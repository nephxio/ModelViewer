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
	ModelViewerSimpleRenderSystem::ModelViewerSimpleRenderSystem(std::shared_ptr<ModelViewerDevice> device, VkRenderPass renderPass) : modelViewerDevice { device }
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	ModelViewerSimpleRenderSystem::~ModelViewerSimpleRenderSystem()
	{
		vkDestroyPipelineLayout(modelViewerDevice->device(), pipelineLayout, nullptr);
	}

	void ModelViewerSimpleRenderSystem::createPipelineLayout()
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

	void ModelViewerSimpleRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		ModelViewerPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		modelViewerPipeline = std::make_unique<ModelViewerPipeline>(*modelViewerDevice,
			"..\\src\\shaders\\simple_shader.vert.spv",
			"..\\src\\shaders\\simple_shader.frag.spv",
			pipelineConfig);

	}

	void ModelViewerSimpleRenderSystem::renderModelObjects(VkCommandBuffer commandBuffer, std::vector<ModelViewerObject>& modelObjects, const ModelViewerCamera& camera)
	{
		modelViewerPipeline->bind(commandBuffer);

		auto projectionView = camera.getProjection() * camera.getView();

		for (auto& object : modelObjects)
		{
			SimplePushConstantData push{};
			push.color = object.color;
			push.transform = projectionView * object.transform.mat4();

			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			object.model->bind(commandBuffer);
			object.model->draw(commandBuffer);
		}
	}
} // namespace ModelViewer
