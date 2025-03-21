#ifndef VULKAN_APPLICATION_TEXTURE_MANAGER
#define VULKAN_APPLICATION_TEXTURE_MANAGER

#include "VulkanApplicationHelpers.h"
#include <stb_image.h>

class VulkanApplicationTextureManager {
	private:
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
	public:
		VulkanApplicationTextureManager(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
		~VulkanApplicationTextureManager();
		void cleanup(VkDevice logicalDevice);
		void createTextureImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
		void createTextureImageView(VkDevice logicalDevice);
		void createTextureSampler(VkDevice logicalDevice, VkPhysicalDevice physicalDevice);
		VkImage getTextureImage();
		VkDeviceMemory getTextureImageMemory();
		VkImageView getTextureImageView();
		VkSampler getTextureSampler();
};

#endif