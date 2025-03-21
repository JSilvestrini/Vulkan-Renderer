#ifndef VULKAN_APPLICATION_INSTANCE_MANAGER
#define VULKAN_APPLICATION_INSTANCE_MANAGER

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanApplicationHelpers.h"

class VulkanApplicationInstanceManager {
	private:
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
	public:
		VulkanApplicationInstanceManager();
		~VulkanApplicationInstanceManager();
		void cleanup();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
		void setupDebugMessenger();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		VkInstance getInstance();
};

#endif