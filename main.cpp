#define STB_IMAGE_IMPLEMENTATION
#include "VulkanApplication.h"

int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	} catch (const std::exception& e) {
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
PRESENT_MODE_FIFO:			V-SYNC - ALWAYS AVAILABLE
PRESENT_MODE_FIFO_RELAXED: NO V-SYNC
PRESENT_MODE_IMMEDIATE:	NO VSYNC
PRESENT_MODE_MAILBOX:		V-SYNC + LESS LATENCY
*/