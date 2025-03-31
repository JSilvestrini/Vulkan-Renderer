#include "headers/VulkanApplicationBufferManager.h"

VulkanApplicationBufferManager::VulkanApplicationBufferManager(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool) {
	createVertexBuffer(logicalDevice, physicalDevice, graphicsQueue, commandPool);
	createIndexBuffer(logicalDevice, physicalDevice, graphicsQueue, commandPool);
	createUniformBuffers(logicalDevice, physicalDevice);
}

VulkanApplicationBufferManager::~VulkanApplicationBufferManager() {}

void VulkanApplicationBufferManager::cleanup(VkDevice logicalDevice) {
	for (size_t i = 0; i < kMAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(logicalDevice, uniformBuffers[i], nullptr);
		vkFreeMemory(logicalDevice, uniformBuffersMemories[i], nullptr);
	}

	vkDestroyBuffer(logicalDevice, indexBuffer, nullptr);
	vkFreeMemory(logicalDevice, indexBufferMemory, nullptr);
	vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);
	vkFreeMemory(logicalDevice, vertexBufferMemory, nullptr);
}

void VulkanApplicationBufferManager::createUniformBuffers(VkDevice logicalDevice, VkPhysicalDevice physicalDevice) {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(kMAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemories.resize(kMAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(kMAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < kMAX_FRAMES_IN_FLIGHT; i++) {
		createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemories[i]);

		vkMapMemory(logicalDevice, uniformBuffersMemories[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}
}

void VulkanApplicationBufferManager::createVertexBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool) {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(logicalDevice, stagingBufferMemory);

	createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize, logicalDevice, graphicsQueue, commandPool);

	vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void VulkanApplicationBufferManager::createIndexBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool) {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(logicalDevice, stagingBufferMemory);

	createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize, logicalDevice, graphicsQueue, commandPool);

	vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void VulkanApplicationBufferManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice logicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(logicalDevice, commandPool);

	VkBufferCopy copyRegion{};
	// srcOffset and dstOffset are optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer, commandPool, logicalDevice, graphicsQueue);
}

void VulkanApplicationBufferManager::updateUniformBuffer(uint32_t currentImage, VkExtent2D swapchainExtent) {
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.projection = glm::perspective(glm::radians(45.0f), (swapchainExtent.width / (float)swapchainExtent.height), 0.1f, 10.0f);

	ubo.projection[1][1] *= -1; // flip y since vulkan is upside down
	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

VkBuffer VulkanApplicationBufferManager::getVertexBuffer() {
	return this->vertexBuffer;
}

VkDeviceMemory VulkanApplicationBufferManager::getVertexBufferMemory() {
	return this->vertexBufferMemory;
}

VkBuffer VulkanApplicationBufferManager::getIndexBuffer() {
	return this->indexBuffer;
}

VkDeviceMemory VulkanApplicationBufferManager::getIndexBufferMemory() {
	return this->indexBufferMemory;
}

std::vector<VkBuffer> VulkanApplicationBufferManager::getUniformBuffers() {
	return this->uniformBuffers;
}

std::vector<VkDeviceMemory> VulkanApplicationBufferManager::getUniformBuffersMemories() {
	return this->uniformBuffersMemories;
}

std::vector<void*> VulkanApplicationBufferManager::getUniformBuffersMapped() {
	return this->uniformBuffersMapped;
}

std::vector<uint16_t> VulkanApplicationBufferManager::getIndices() {
	return this->indices;
}