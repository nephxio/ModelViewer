#pragma once

#include "ModelViewerWindow.h"
#include "ModelViewerPipeline.h"

namespace ModelViewer
{
	class ModelViewer
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();
	private:
		ModelViewerWindow modelViewerWindow{WIDTH, HEIGHT, "Model Viewer"};
		ModelViewerPipeline modelViewerPipeline{ "src\\shaders\\simple_shader.vert.spv", "src\\shaders\\simple_shader.frag.spv" };
	};
}