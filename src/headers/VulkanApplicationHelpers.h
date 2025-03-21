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
#include <optional>

#include <memory>

using std::cout, std::endl, std::cerr;

/*****************************************************
					CONSTANTS
*****************************************************/
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

/**************************************************
					STRUCTS
***************************************************/
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapchainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

/****************************************************
				HELPER FUNCTIONS
*****************************************************/

std::vector<const char*> getRequiredExtensions();
bool checkValidationLayerSupport();
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice pDevice, VkSurfaceKHR surface);
SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
#endif