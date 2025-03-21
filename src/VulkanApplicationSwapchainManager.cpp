#include "headers/VulkanApplicationSwapchainManager.h"

VulkanApplicationSwapchainManager::VulkanApplicationSwapchainManager(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window) {
	createSwapchain(physicalDevice, logicalDevice, surface, window);
	createImageViews(logicalDevice);
}

VulkanApplicationSwapchainManager::~VulkanApplicationSwapchainManager() {}

void VulkanApplicationSwapchainManager::cleanup(VkDevice logicalDevice) {
	for (size_t i = 0; i < swapchainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(logicalDevice, swapchainFramebuffers[i], nullptr);
	}

	for (size_t i = 0; i < swapchainImageViews.size(); i++) {
		vkDestroyImageView(logicalDevice, swapchainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
}

VkSwapchainKHR VulkanApplicationSwapchainManager::getSwapchain() {
	return this->swapchain;
}

std::vector<VkImage> VulkanApplicationSwapchainManager::getSwapchainImages() {
	return this->swapchainImages;
}

VkFormat VulkanApplicationSwapchainManager::getSwapchainImageFormat() {
	return this->swapchainImageFormat;
}

VkExtent2D VulkanApplicationSwapchainManager::getSwapchainExtent() {
	return this->swapchainExtent;
}

std::vector<VkImageView> VulkanApplicationSwapchainManager::getSwapchainImageViews() {
	return this->swapchainImageViews;
}

std::vector<VkFramebuffer> VulkanApplicationSwapchainManager::getSwapchainFramebuffers() {
	return this->swapchainFramebuffers;
}

void VulkanApplicationSwapchainManager::recreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window, VkRenderPass renderPass) {
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);

	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(logicalDevice);

	cleanup(logicalDevice);

	createSwapchain(physicalDevice, logicalDevice, surface, window);
	createImageViews(logicalDevice);
	createFrameBuffer(logicalDevice, renderPass);
}

void VulkanApplicationSwapchainManager::createImageViews(VkDevice logicalDevice) {
	swapchainImageViews.resize(swapchainImages.size());

	for (size_t i = 0; i < swapchainImages.size(); i++) {
		swapchainImageViews[i] = createImageView(swapchainImages[i], swapchainImageFormat, logicalDevice);
	}
}

void VulkanApplicationSwapchainManager::createSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window) {
	SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapchainSupport.capabilities, window);

	uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

	if (swapchainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapchainSupport.capabilities.maxImageCount) {
		imageCount = swapchainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Swapchain");
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, swapchainImages.data());

	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;
}

void VulkanApplicationSwapchainManager::createFrameBuffer(VkDevice logicalDevice, VkRenderPass renderPass) {
	swapchainFramebuffers.resize(swapchainImageViews.size());

	for (size_t i = 0; i < swapchainImageViews.size(); i++) {
		VkImageView attachments[] = {
			swapchainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};

		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapchainExtent.width;
		framebufferInfo.height = swapchainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Create Framebuffer Object");
		}
	}
}

VkSurfaceFormatKHR VulkanApplicationSwapchainManager::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR VulkanApplicationSwapchainManager::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanApplicationSwapchainManager::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
	// resolution we are drawing to
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}

	int width;
	int height;
	glfwGetFramebufferSize(window, &width, &height);

	VkExtent2D actualExtent = {
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
	};

	actualExtent.width = std::clamp(actualExtent.width,
		capabilities.minImageExtent.width,
		capabilities.maxImageExtent.width);

	actualExtent.height = std::clamp(actualExtent.height,
		capabilities.minImageExtent.height,
		capabilities.maxImageExtent.height);

	return actualExtent;
}
