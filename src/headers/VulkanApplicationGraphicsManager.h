#ifndef VULKAN_APPLICATION_GRAPHICS_MANAGER
#define VULKAN_APPLICATION_GRAPHICS_MANAGER

#include "VulkanApplicationHelpers.h"

class VulkanApplicationGraphicsManager {
	private:
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
	public:
		VulkanApplicationGraphicsManager(VkFormat swapchainImageFormat, VkDevice logicalDevice, VkPhysicalDevice physicalDevice);
		~VulkanApplicationGraphicsManager();
		void cleanup(VkDevice logicalDevice);
		VkRenderPass getRenderPass();
		VkPipelineLayout getPipelineLayout();
		VkPipeline getGraphicsPipeline();
		void createRenderPass(VkFormat swapchainImageFormat, VkDevice logicalDevice,  VkPhysicalDevice physicalDevice);
		void createGraphicsPipeline(VkDevice logicalDevice, VkDescriptorSetLayout descriptorSetLayout);
		VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice logicalDevice);
};

#endif