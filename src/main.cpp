#include "ModelViewer.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
	ModelViewer::ModelViewer modelViewer{};

	try
	{
		modelViewer.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;
}