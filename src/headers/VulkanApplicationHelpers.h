#ifndef VULKAN_APPLICATION_HELPERS
#define VULKAN_APPLICATION_HELPERS

/*	Contains random structs and functions that are
	used across different sections of the program.

	Code is based on vulkan-tutorial.com, just broken
	into its own class for easier debugging and editing.
*/

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <vector>

#include <memory>

using std::cout, std::endl, std::cerr;

/**************************
		CONSTANTS
***************************/
const uint32_t kWIDTH = 800;
const uint32_t kHEIGHT = 600;
const bool debug = true;
const int kMAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::vector<const char*> getRequiredExtensions();
bool checkValidationLayerSupport();

#endif