#include "ImGuiRenderer.h"

#include "ModelViewerDevice.h"
#include "ModelViewerRenderer.h"
#include "ModelViewerWindow.h"

namespace ModelViewer
{
	ImGuiRenderer::ImGuiRenderer(std::shared_ptr<ModelViewerDevice> device, std::shared_ptr<ModelViewerWindow> window, std::shared_ptr<ModelViewerRenderer> renderer)
		: modelViewerDevice { device }, modelViewerWindow { window }, modelViewerRenderer { renderer }
	{
		createDescriptorPool();
		init();
	}

	ImGuiRenderer::~ImGuiRenderer()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		vkDestroyDescriptorPool(modelViewerDevice->device(), descriptorPool, nullptr);
		ImGui::DestroyContext();
	}

	void ImGuiRenderer::createDescriptorPool()
	{
		// Create a descriptor pool for ImGui
		VkDescriptorPoolSize poolSizes[] = {
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 }
		};

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1000;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = poolSizes;

		if (vkCreateDescriptorPool(modelViewerDevice->device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create ImGui descriptor pool");
		}

	}
	void ImGuiRenderer::init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		io = &ImGui::GetIO(); (void)io;
		(*io).ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		(*io).ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		(*io).ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		(*io).ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if ((*io).ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForVulkan(modelViewerWindow->getGLFWWindow(), true);

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = modelViewerDevice->getInstance();
		init_info.PhysicalDevice = modelViewerDevice->getPhysicalDevice();
		init_info.Device = modelViewerDevice->device();
		init_info.QueueFamily = modelViewerDevice->findPhysicalQueueFamilies().graphicsFamily;
		init_info.Queue = modelViewerDevice->graphicsQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = descriptorPool;
		init_info.RenderPass = modelViewerRenderer->getSwapChain()->getRenderPass();
		init_info.Subpass = 0;
		init_info.MinImageCount = 2;
		init_info.ImageCount = modelViewerRenderer->getSwapChain()->imageCount();
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = VK_NULL_HANDLE;
		ImGui_ImplVulkan_Init(&init_info);
	}
	void ImGuiRenderer::drawDemo(bool show_demo_window, bool show_another_window, ImVec4 clear_color)
	{
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
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

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / (*io).Framerate, (*io).Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		ImDrawData* main_draw_data = ImGui::GetDrawData();

		ImGui_ImplVulkan_RenderDrawData(main_draw_data, modelViewerRenderer->getCurrentCommandBuffer());
	}
} // namespace ModelViewer