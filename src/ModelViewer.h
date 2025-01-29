#pragma once

#include "ModelViewerWindow.h"
#include "ModelViewerPipeline.h"

#include <iostream>

namespace ModelViewer
{
	class ModelViewer
	{
	public:
		ModelViewer();

		void run();
	private:
		void initialize();

		int WIDTH;
		int HEIGHT;

		GLFWmonitor* primaryMonitor;
		const GLFWvidmode* mode;
		std::unique_ptr<ModelViewerDevice> modelViewerDevice;
		std::unique_ptr<ModelViewerWindow> modelViewerWindow; 
		std::unique_ptr<ModelViewerPipeline> modelViewerPipeline;
	};
}