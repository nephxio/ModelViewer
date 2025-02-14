#pragma once

#include "ModelViewerModel.h"

#include <memory>

namespace ModelViewer
{
	struct Transform2DComponent
	{
		glm::vec2 translation{};
		glm::vec2 scale{ 1.0f, 1.0f };
		float rotation;

		glm::mat2 mat2() 
		{ 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotationMat{ {c, s}, {-s, c} };
			glm::mat2 scaleMat{ {scale.x, 0.0f}, {0, scale.y} };
			return scaleMat * rotationMat; 
		}
	};
	class ModelViewerObject
	{
	public:
		using id_t = unsigned int;

		static ModelViewerObject createGameObject()
		{
			static id_t currentId = 0;
			return ModelViewerObject{ currentId++ };
		}

		ModelViewerObject(const ModelViewerObject&) = delete;
		ModelViewerObject &operator=(const ModelViewerObject&) = delete;
		ModelViewerObject(ModelViewerObject&&) = default;
		ModelViewerObject& operator=(ModelViewerObject&&) = default;

		id_t getId() const { return id; }

		std::shared_ptr<ModelViewerModel> model{};
		glm::vec3  color{};
		Transform2DComponent transform2d;

	private:
		ModelViewerObject(id_t objId) : id{ objId } {};
		id_t id;
	};
} // namespace ModelViewer