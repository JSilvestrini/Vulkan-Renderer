#include "headers/VulkanApplicationTextureManager.h"

VulkanApplicationTextureManager::VulkanApplicationTextureManager(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue) {
	createTextureImage(logicalDevice, physicalDevice, commandPool, graphicsQueue);
	createTextureImageView(logicalDevice);
	createTextureSampler(logicalDevice, physicalDevice);
}

VulkanApplicationTextureManager::~VulkanApplicationTextureManager() {}

void VulkanApplicationTextureManager::cleanup(VkDevice logicalDevice) {
	vkDestroySampler(logicalDevice, textureSampler, nullptr);
	vkDestroyImageView(logicalDevice, textureImageView, nullptr);
	vkDestroyImage(logicalDevice, textureImage, nullptr);
	vkFreeMemory(logicalDevice, textureImageMemory, nullptr);
}

VkImage VulkanApplicationTextureManager::getTextureImage() {
	return this->textureImage;
}

VkDeviceMemory VulkanApplicationTextureManager::getTextureImageMemory() {
	return this->textureImageMemory;
}

VkImageView VulkanApplicationTextureManager::getTextureImageView() {
	return this->textureImageView;
}

VkSampler VulkanApplicationTextureManager::getTextureSampler() {
	return this->textureSampler;
}

void VulkanApplicationTextureManager::createTextureImageView(VkDevice logicalDevice) {
	textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, logicalDevice);
}

void VulkanApplicationTextureManager::createTextureSampler(VkDevice logicalDevice, VkPhysicalDevice physicalDevice) {
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR; // helps with oversampling
	samplerInfo.minFilter = VK_FILTER_LINEAR; // helps with undersampling

	// UVW = XYZ, what to do with image once we hit the bounds of it
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE; // mainly used for percentage-closer filtering on shadow maps
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(logicalDevice, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Texture Sampler");
	}
}

void VulkanApplicationTextureManager::createTextureImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue) {
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("textures/Statue_Image.jpg", &texWidth, &texHeight, &texChannels,
		STBI_rgb_alpha);

	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("Failed to Load Texture");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(logicalDevice, physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, (size_t)imageSize);
	vkUnmapMemory(logicalDevice, stagingBufferMemory);

	stbi_image_free(pixels);

	createImage(static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		textureImage, textureImageMemory, logicalDevice, physicalDevice);

	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandPool, logicalDevice, graphicsQueue);
	copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), logicalDevice, commandPool, graphicsQueue);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandPool, logicalDevice, graphicsQueue);

	vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}