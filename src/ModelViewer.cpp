#include "ModelViewer.h"
#include "Renderer/ModelViewerSimpleRenderSystem.h"
#include "Camera/ModelViewerCamera.h"
#include "Input/ModelViewerKeyboardController.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <cassert>
#include <stdexcept>
#include <array>
#include <chrono>

namespace ModelViewer
{	
	static void check_vk_result(VkResult err)
	{
		if (err == 0)
			return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0)
			abort();
	}

	ModelViewer::ModelViewer()
	{
		primaryMonitor = glfwGetPrimaryMonitor();
		if (!primaryMonitor)
		{
			throw std::runtime_error("Failed to get primary monitor!");
		}

		mode = glfwGetVideoMode(primaryMonitor);
		if (!mode)
		{
			throw std::runtime_error("Failed to get video mode!");
		}

		WIDTH = static_cast<int>(mode->width * 0.75);
		HEIGHT = static_cast<int>(mode->height * 0.75);

		modelViewerWindow = std::make_shared<ModelViewerWindow>(WIDTH, HEIGHT, "Vulkan Window");
		modelViewerDevice = std::make_shared<ModelViewerDevice>(*modelViewerWindow);
		modelViewerRenderer = std::make_shared<ModelViewerRenderer>(modelViewerWindow, modelViewerDevice);

		loadModelObjects();

	}

	ModelViewer::~ModelViewer()
	{
	}

	std::unique_ptr<ModelViewerModel> createCubeModel(ModelViewerDevice& device, glm::vec3 offset) {
		ModelViewerModel::Builder modelBuilder{};
		modelBuilder.vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};
		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}

		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<ModelViewerModel>(device, modelBuilder);
	}

	void ModelViewer::loadModelObjects()
	{
		std::shared_ptr<ModelViewerModel> cubeModel = createCubeModel(*modelViewerDevice, { 0.0f, 0.0f, 0.0f });

		auto cube = ModelViewerObject::createObject();
		cube.model = cubeModel;

		cube.transform.translation = { 0.0f, 0.0f, 2.5f };
		cube.transform.scale = { 0.5f, 0.5f, 0.5f };
	
		modelObjects.push_back(std::move(cube));
	}

	void ModelViewer::run()
	{
		ImGuiRenderer imguiRenderer{ modelViewerDevice, modelViewerWindow, modelViewerRenderer };
		ModelViewerSimpleRenderSystem simpleRenderSystem{ modelViewerDevice, modelViewerRenderer->getSwapChainRenderPass() };
		ModelViewerCamera camera{};

		//camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));

		auto viewerObject = ModelViewerObject::createObject();
		ModelViewerKeyboardController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!modelViewerWindow->shouldClose())
		{
			glfwPollEvents();
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();

			currentTime = newTime;

			//cameraController.moveInPlaneXZ(modelViewerWindow->getGLFWWindow(), frameTime, viewerObject);
			//camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			//float aspect = modelViewerRenderer->getAspectRatio();
			//camera.setPerspectiveProjection(glm::radians(50.0f), modelViewerWindow->getWidth(), modelViewerWindow->getHeight(), 0.1f, 10.0f);

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ModelViewerObject* objectptr = &modelObjects[0];

			if (auto commandBuffer = modelViewerRenderer->beginFrame())
			{
				modelViewerRenderer->beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderModelObjects(commandBuffer, modelObjects, camera);
				imguiRenderer.renderUI(objectptr);
				imguiRenderer.drawUI();
				modelViewerRenderer->endSwapChainRenderPass(commandBuffer);
				modelViewerRenderer->endFrame();
			}

			// Update and Render additional Platform Windows
			if (imguiRenderer.getImGuiIO()->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}

		vkDeviceWaitIdle(modelViewerDevice->device());
	}
}
