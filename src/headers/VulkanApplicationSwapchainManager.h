#ifndef VULKAN_APPLICATION_SWAPCHAIN_MANAGER
#define VULKAN_APPLICATION_SWAPCHAIN_MANAGER

#include "VulkanApplicationHelpers.h"

class VulkanApplicationSwapchainManager {
	private:
		VkSwapchainKHR swapchain;
		std::vector<VkImage> swapchainImages;
		VkFormat swapchainImageFormat;
		VkExtent2D swapchainExtent;
		std::vector<VkImageView> swapchainImageViews;
		std::vector<VkFramebuffer> swapchainFramebuffers;
		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;
	public:
		VulkanApplicationSwapchainManager(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window);
		~VulkanApplicationSwapchainManager();
		void cleanup(VkDevice logicalDevice);
		VkSwapchainKHR getSwapchain();
		std::vector<VkImage> getSwapchainImages();
		VkFormat getSwapchainImageFormat();
		VkExtent2D getSwapchainExtent();
		std::vector<VkImageView> getSwapchainImageViews();
		std::vector<VkFramebuffer> getSwapchainFramebuffers();
		void createDepthResources(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool);

		void createImageViews(VkDevice logicalDevice);
		void createSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
		void recreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window, VkRenderPass renderPass, VkQueue graphicsQueue, VkCommandPool commandPool);
		void createFrameBuffer(VkDevice logicalDevice, VkRenderPass renderPass);
};

#endif