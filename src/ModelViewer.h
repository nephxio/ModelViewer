#pragma once

#include "ModelViewerDevice.h"
#include "ModelViewerWindow.h"
#include "ModelViewerRenderer.h"
#include "ModelViewerObject.h"

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