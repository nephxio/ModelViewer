#include "ModelViewer.h"

namespace ModelViewer
{
	void ModelViewer::run()
	{
		while (!modelViewerWindow.shouldClose())
		{
			glfwPollEvents();
		}
	}
}