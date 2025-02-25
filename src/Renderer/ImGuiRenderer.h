#pragma once

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <stdio.h>
#include <memory>

namespace ModelViewer
{
	class ModelViewerDevice;
	class ModelViewerWindow;
	class ModelViewerRenderer;

	class ImGuiRenderer
	{
	public:
		ImGuiRenderer(std::shared_ptr<ModelViewerDevice> device, std::shared_ptr<ModelViewerWindow> window, std::shared_ptr<ModelViewerRenderer> renderer);
		~ImGuiRenderer();

		void createDescriptorPool();

		void init();

		void drawDemo(bool show_demo_window, bool show_another_window, ImVec4 clear_color);

		ImGuiIO* getImGuiIO() { return io; }

	private:
		ImGuiIO* io;

		std::shared_ptr<ModelViewerDevice> modelViewerDevice;
		std::shared_ptr<ModelViewerWindow> modelViewerWindow;
		std::shared_ptr<ModelViewerRenderer> modelViewerRenderer;

		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	};
} // namespace ModelViewer