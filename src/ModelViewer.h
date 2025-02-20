#pragma once

#include "ModelViewerDevice.h"
#include "ModelViewerWindow.h"
#include "Renderer/ModelViewerRenderer.h"
#include "ModelViewerObject.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <memory>
#include <vector>

namespace ModelViewer
{
	class ModelViewer
	{
	public:
		ModelViewer();
		~ModelViewer();

		ModelViewer(const ModelViewer&) = delete;
		ModelViewer &operator=(const ModelViewer&) = delete;

		void init_imgui();
		void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
		void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);
		void FramePresent(ImGui_ImplVulkanH_Window* wd);
		void run();
	private:

		void loadModelObjects();
		
		int WIDTH;
		int HEIGHT;

		GLFWmonitor* primaryMonitor;
		const GLFWvidmode* mode;

		std::shared_ptr<ModelViewerWindow> modelViewerWindow;
		std::shared_ptr<ModelViewerDevice> modelViewerDevice;
		std::unique_ptr<ModelViewerRenderer> modelViewerRenderer;
		std::vector<ModelViewerObject> modelObjects;
	};
}