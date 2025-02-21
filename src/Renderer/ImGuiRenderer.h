#pragma once

#include "ModelViewerDevice.h"
#include "ModelViewerRenderer.h"
#include "ModelViewerWindow.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <stdio.h>
#include <memory>

namespace ModelViewer
{
	class ImGuiRenderer
	{
	public:
		ImGuiRenderer(std::shared_ptr<ModelViewerDevice> device, std::shared_ptr<ModelViewerWindow> window, std::shared_ptr<ModelViewerRenderer> renderer);
		~ImGuiRenderer();

		void init_imgui();

		void setupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);

		void renderFrame(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);

		void presentFrame(ImGui_ImplVulkanH_Window* wd);

		void cleanupVulkanWindow();

		VkAllocationCallbacks* getAllocator() { return allocator; }
		VkPipelineCache getPipelineCache() { return pipelineCache; }
		VkDescriptorPool getDescriptorPool() { return descriptorPool; }
		ImGui_ImplVulkanH_Window getMainWindowData() { return mainWindowData; }
		ImGui_ImplVulkanH_Window* getWindow() { return wd; }
		uint32_t getMinImageCount() { return minImageCount; }
		bool isSwapChainRebuild() { return swapChainRebuild; }

		void showDemoWindow(ImGuiIO& io, bool show_demo_window, bool show_another_window, ImVec4 clear_color);

	private:
		std::shared_ptr<ModelViewerDevice> modelViewerDevice;
		std::shared_ptr<ModelViewerRenderer> modelViewerRenderer;
		std::shared_ptr<ModelViewerWindow> modelViewerWindow;

		VkAllocationCallbacks*   allocator = nullptr;
		VkPipelineCache          pipelineCache = VK_NULL_HANDLE;
		VkDescriptorPool         descriptorPool = VK_NULL_HANDLE;

		ImGui_ImplVulkanH_Window mainWindowData;
		ImGui_ImplVulkanH_Window* wd;
		uint32_t                 minImageCount;
		bool                     swapChainRebuild = false;
	};
} // namespace ModelViewer