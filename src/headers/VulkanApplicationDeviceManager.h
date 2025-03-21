#ifndef VULKAN_APPLICATION_DEVICE_MANAGER
#define VULKAN_APPLICATION_DEVICE_MANAGER

#include "VulkanApplicationHelpers.h"
#include <set>

class VulkanApplicationDeviceManager {
	private:
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice logicalDevice;
		VkQueue presentQueue;
		VkQueue graphicsQueue;
		void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
	public:
		VulkanApplicationDeviceManager(VkInstance instance, VkSurfaceKHR surface);
		~VulkanApplicationDeviceManager();
		void cleanup();
		VkPhysicalDevice getPhysicalDevice();
		VkDevice getLogicalDevice();
		bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
		void createLogicalDevice(VkSurfaceKHR surface);
		VkQueue getGraphicsQueue();
		VkQueue getPresentQueue();
};

#endif