#pragma once

#include "ModelViewerDevice.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace ModelViewer
{
	class ModelViewerModel
	{
	public:

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		ModelViewerModel(ModelViewerDevice& device, const std::vector<Vertex>& vertices);
		~ModelViewerModel();

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		ModelViewerModel(const ModelViewerModel&) = delete;
		ModelViewerModel& operator=(const ModelViewerModel&) = delete;
			 
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);

		ModelViewerDevice &modelViewerDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
} // namespace ModelViewer
