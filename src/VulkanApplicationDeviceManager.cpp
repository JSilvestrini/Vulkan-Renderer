#include "headers/VulkanApplicationDeviceManager.h"

VulkanApplicationDeviceManager::VulkanApplicationDeviceManager(VkInstance instance, VkSurfaceKHR surface) {
	pickPhysicalDevice(instance, surface);
	createLogicalDevice(surface);
}

VulkanApplicationDeviceManager::~VulkanApplicationDeviceManager() {}

void VulkanApplicationDeviceManager::cleanup() {
	vkDestroyDevice(logicalDevice, nullptr);
}

void VulkanApplicationDeviceManager::pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to Find Vulkan Compatable GPUs");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	// could remove this loop and instead create an ordered map to store each device in
	// then rate each device based on score and use the highest score
	for (const auto& physicalDevice : devices) {
		if (isDeviceSuitable(physicalDevice, surface)) {
			this->physicalDevice = physicalDevice;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to Find a Suitable GPU");
	}
}

bool VulkanApplicationDeviceManager::isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
	// can instead get the physicalDevice properties and features and return based on that
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

	bool extensionSupported = checkDeviceExtensionSupport(physicalDevice);
	bool swapchainAdequate = false;

	if (extensionSupported) {
		SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice, surface);
		swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

	return indices.isComplete() && extensionSupported && swapchainAdequate && supportedFeatures.samplerAnisotropy;
}

bool VulkanApplicationDeviceManager::checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void VulkanApplicationDeviceManager::createLogicalDevice(VkSurfaceKHR surface) {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = queueCreateInfos.size();
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (debug) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Logical Device");
	}

	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}

VkPhysicalDevice VulkanApplicationDeviceManager::getPhysicalDevice() {
	return this->physicalDevice;
}

VkDevice VulkanApplicationDeviceManager::getLogicalDevice() {
	return this->logicalDevice;
}

VkQueue VulkanApplicationDeviceManager::getGraphicsQueue() {
	return this->graphicsQueue;
}

VkQueue VulkanApplicationDeviceManager::getPresentQueue() {
	return this->presentQueue;
}