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
	static bool                     g_SwapChainRebuild = false;
	static ImGui_ImplVulkanH_Window g_MainWindowData;



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
		std::vector<ModelViewerModel::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<ModelViewerModel>(device, vertices);
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
		ImGuiRenderer imGuiRenderer{ modelViewerDevice, modelViewerWindow, modelViewerRenderer };
		ModelViewerSimpleRenderSystem simpleRenderSystem{ modelViewerDevice, modelViewerRenderer->getSwapChainRenderPass() };
		ModelViewerCamera camera{};

		camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));

		auto viewerObject = ModelViewerObject::createObject();
		ModelViewerKeyboardController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		imGuiRenderer.init_imgui();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(modelViewerWindow->getGLFWWindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		//init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
		init_info.Instance = modelViewerDevice->getInstance();
		init_info.PhysicalDevice = modelViewerDevice->getPhysicalDevice();
		init_info.Device = modelViewerDevice->device();
		init_info.QueueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(modelViewerDevice->getPhysicalDevice());
		init_info.Queue = modelViewerDevice->graphicsQueue();
		init_info.PipelineCache = imGuiRenderer.getPipelineCache();
		init_info.DescriptorPool = imGuiRenderer.getDescriptorPool();
		init_info.RenderPass = imGuiRenderer.getWindow()->RenderPass;
		init_info.Subpass = 0;
		init_info.MinImageCount = imGuiRenderer.getMinImageCount();
		init_info.ImageCount = imGuiRenderer.getWindow()->ImageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = imGuiRenderer.getAllocator();
		init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&init_info);

		// Our state
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		while (!modelViewerWindow->shouldClose())
		{
			glfwPollEvents();
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();

			currentTime = newTime;

			cameraController.moveInPlaneXZ(modelViewerWindow->getGLFWWindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = modelViewerRenderer->getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

			int fb_width, fb_height;
			glfwGetFramebufferSize(modelViewerWindow->getGLFWWindow(), &fb_width, &fb_height);
			if (fb_width > 0 && fb_height > 0 && (g_SwapChainRebuild || g_MainWindowData.Width != fb_width || g_MainWindowData.Height != fb_height))
			{
				ImGui_ImplVulkan_SetMinImageCount(imGuiRenderer.getMinImageCount());
				ImGui_ImplVulkanH_CreateOrResizeWindow(modelViewerDevice->getInstance(), 
					modelViewerDevice->getPhysicalDevice(), 
					modelViewerDevice->device(), 
					imGuiRenderer.getWindow(),
					ImGui_ImplVulkanH_SelectQueueFamilyIndex(modelViewerDevice->getPhysicalDevice()), 
					imGuiRenderer.getAllocator(), 
					fb_width, 
					fb_height, imGuiRenderer.getMinImageCount());
				g_MainWindowData.FrameIndex = 0;
				g_SwapChainRebuild = false;
			}
			if (glfwGetWindowAttrib(modelViewerWindow->getGLFWWindow(), GLFW_ICONIFIED) != 0)
			{
				ImGui_ImplGlfw_Sleep(10);
				continue;
			}

			// Start the Dear ImGui frame
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();

			ImGui::NewFrame();

			imGuiRenderer.showDemoWindow(io, show_demo_window, show_another_window, clear_color);

			if (auto commandBuffer = modelViewerRenderer->beginFrame())
			{
				modelViewerRenderer->beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderModelObjects(commandBuffer, modelObjects, camera);
				modelViewerRenderer->endSwapChainRenderPass(commandBuffer);
				modelViewerRenderer->endFrame();
			}
		}

		vkDeviceWaitIdle(modelViewerDevice->device());
	}
}
