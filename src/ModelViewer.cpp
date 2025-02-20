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
		modelViewerRenderer = std::make_unique<ModelViewerRenderer>(modelViewerWindow, modelViewerDevice);

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

	void ModelViewer::init_imgui()
	{

	}

	// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
	// Your real engine/app may not use them.
	void ModelViewer::SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
	{
		wd->Surface = surface;

		// Check for WSI support
		VkBool32 res;
		vkGetPhysicalDeviceSurfaceSupportKHR(modelViewerDevice->getPhysicalDevice(), ImGui_ImplVulkanH_SelectQueueFamilyIndex(modelViewerDevice->getPhysicalDevice()), wd->Surface, &res);
		if (res != VK_TRUE)
		{
			fprintf(stderr, "Error no WSI support on physical device 0\n");
			exit(-1);
		}

		// Select Surface Format
		const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(modelViewerDevice->getPhysicalDevice(), wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

		// Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
		wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(modelViewerDevice->getPhysicalDevice(), wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
		//printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

		// Create SwapChain, RenderPass, Framebuffer, etc.
		ImGui_ImplVulkanH_CreateOrResizeWindow(
			modelViewerDevice->getInstance(), 
			modelViewerDevice->getPhysicalDevice(), 
			modelViewerDevice->device(), 
			wd, 
			ImGui_ImplVulkanH_SelectQueueFamilyIndex(modelViewerDevice->getPhysicalDevice()), 
			nullptr, 
			width, 
			height, 
			2);
	}

	void ModelViewer::FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
	{
		VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
		VkResult err = vkAcquireNextImageKHR(modelViewerDevice->device(), wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
			g_SwapChainRebuild = true;
		if (err == VK_ERROR_OUT_OF_DATE_KHR)
			return;
		if (err != VK_SUBOPTIMAL_KHR)
			check_vk_result(err);

		ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
		{
			err = vkWaitForFences(modelViewerDevice->device(), 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
			check_vk_result(err);

			err = vkResetFences(modelViewerDevice->device(), 1, &fd->Fence);
			check_vk_result(err);
		}
		{
			err = vkResetCommandPool(modelViewerDevice->device(), fd->CommandPool, 0);
			check_vk_result(err);
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
			check_vk_result(err);
		}
		{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = wd->RenderPass;
			info.framebuffer = fd->Framebuffer;
			info.renderArea.extent.width = wd->Width;
			info.renderArea.extent.height = wd->Height;
			info.clearValueCount = 1;
			info.pClearValues = &wd->ClearValue;
			vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

		// Submit command buffer
		vkCmdEndRenderPass(fd->CommandBuffer);
		{
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.waitSemaphoreCount = 1;
			info.pWaitSemaphores = &image_acquired_semaphore;
			info.pWaitDstStageMask = &wait_stage;
			info.commandBufferCount = 1;
			info.pCommandBuffers = &fd->CommandBuffer;
			info.signalSemaphoreCount = 1;
			info.pSignalSemaphores = &render_complete_semaphore;

			err = vkEndCommandBuffer(fd->CommandBuffer);
			check_vk_result(err);
			err = vkQueueSubmit(modelViewerDevice->graphicsQueue(), 1, &info, fd->Fence);
			check_vk_result(err);
		}
	}

	void ModelViewer::FramePresent(ImGui_ImplVulkanH_Window* wd)
	{
		if (g_SwapChainRebuild)
			return;
		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &wd->Swapchain;
		info.pImageIndices = &wd->FrameIndex;
		VkResult err = vkQueuePresentKHR(modelViewerDevice->graphicsQueue(), &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
			g_SwapChainRebuild = true;
		if (err == VK_ERROR_OUT_OF_DATE_KHR)
			return;
		if (err != VK_SUBOPTIMAL_KHR)
			check_vk_result(err);
		wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount; // Now we can use the next set of semaphores
	}

	void ModelViewer::run()
	{
		ModelViewerSimpleRenderSystem simpleRenderSystem{ modelViewerDevice, modelViewerRenderer->getSwapChainRenderPass() };
		ModelViewerCamera camera{};

		camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));

		auto viewerObject = ModelViewerObject::createObject();
		ModelViewerKeyboardController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		VkResult err;
		VkDescriptorPool descriptorPool;
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE },
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 0;
		for (VkDescriptorPoolSize& pool_size : pool_sizes)
			pool_info.maxSets += pool_size.descriptorCount;
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		err = vkCreateDescriptorPool(modelViewerDevice->device(), &pool_info, nullptr, &descriptorPool);
		check_vk_result(err);

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(modelViewerWindow->getGLFWWindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		//init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
		init_info.Instance = modelViewerDevice->getInstance();
		init_info.PhysicalDevice = modelViewerDevice->getPhysicalDevice();
		init_info.Device = modelViewerDevice->device();
		init_info.QueueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(modelViewerDevice->getPhysicalDevice());
		init_info.Queue = modelViewerDevice->graphicsQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = descriptorPool;
		init_info.RenderPass = modelViewerRenderer->getSwapChainRenderPass();
		init_info.Subpass = 0;
		init_info.MinImageCount = 2;
		init_info.ImageCount = modelViewerRenderer->getSwapChain()->imageCount();
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = nullptr;
		init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&init_info);

		// Create Window Surface
		VkSurfaceKHR surface;
		err = glfwCreateWindowSurface(modelViewerDevice->getInstance(), modelViewerWindow->getGLFWWindow(), nullptr, &surface);
		check_vk_result(err);

		// Create Framebuffers
		int w, h;
		glfwGetFramebufferSize(modelViewerWindow->getGLFWWindow(), &w, &h);
		ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
		SetupVulkanWindow(wd, surface, w, h);

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

			//int fb_width, fb_height;
			//glfwGetFramebufferSize(modelViewerWindow->getGLFWWindow(), &fb_width, &fb_height);
			//if (fb_width > 0 && fb_height > 0 && (g_SwapChainRebuild || g_MainWindowData.Width != fb_width || g_MainWindowData.Height != fb_height))
			//{
			//	ImGui_ImplVulkan_SetMinImageCount(2);
			//	ImGui_ImplVulkanH_CreateOrResizeWindow(
			//		modelViewerDevice->getInstance(),
			//		modelViewerDevice->getPhysicalDevice(), 
			//		modelViewerDevice->device(), &g_MainWindowData, ImGui_ImplVulkanH_SelectQueueFamilyIndex(modelViewerDevice->getPhysicalDevice()), nullptr, fb_width, fb_height, 2);
			//	g_MainWindowData.FrameIndex = 0;
			//	g_SwapChainRebuild = false;
			//}
			//if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
			//{
			//	ImGui_ImplGlfw_Sleep(10);
			//	continue;
			//}

			// Start the Dear ImGui frame
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			bool show_demo_window = true;
			bool show_another_window = false;
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

			ImGui::ShowDemoWindow(&show_demo_window);

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
			}

			// 3. Show another simple window.
			{
				ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me"))
					show_another_window = false;
				ImGui::End();
			}

			// Rendering
			ImGui::Render();

			ImDrawData* draw_data = ImGui::GetDrawData();
			const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
			if (!is_minimized)
			{
				wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
				wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
				wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
				wd->ClearValue.color.float32[3] = clear_color.w;
				FrameRender(wd, draw_data);
				FramePresent(wd);
			}

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
