#include "ModelViewerSimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>

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
		float width = (float)modelViewerDevice->getWindow().getWidth();
		float height = (float)modelViewerDevice->getWindow().getHeight();

		auto projectionView = camera.getProjection() * camera.getView();

		for (auto& object : modelObjects)
		{
			glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10));

			SimplePushConstantData push{};
			push.viewProjection = glm::perspectiveFov(glm::radians(45.f), width, height, 0.1f, 100.0f)
				* glm::inverse(cameraTransform);


			push.transform = glm::translate(glm::mat4(1.0f), object.transform.translation) * glm::eulerAngleYXZ(glm::radians(object.transform.rotation.y), glm::radians(object.transform.rotation.x), glm::radians(object.transform.rotation.z));

			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			object.model->bind(commandBuffer);
			object.model->draw(commandBuffer);
		}
	}
} // namespace ModelViewer
