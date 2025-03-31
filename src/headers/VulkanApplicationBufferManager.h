#ifndef VULKAN_APPLICATION_BUFFER_MANAGER
#define VULKAN_APPLICATION_BUFFER_MANAGER

#include "VulkanApplicationHelpers.h"
#include <chrono>
#include <glm/glm.hpp>

class VulkanApplicationBufferManager {
	private:
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemories;
		std::vector<void*> uniformBuffersMapped;

		const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

			{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
		};

		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};
	public:
		VulkanApplicationBufferManager(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool);
		~VulkanApplicationBufferManager();
		void cleanup(VkDevice logicalDevice);
		void createVertexBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool);
		void createIndexBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice logicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool);
		void createUniformBuffers(VkDevice logicalDevice, VkPhysicalDevice physicalDevice);
		void updateUniformBuffer(uint32_t currentImage, VkExtent2D swapchainExtent);
		VkBuffer getVertexBuffer();
		VkDeviceMemory getVertexBufferMemory();
		VkBuffer getIndexBuffer();
		VkDeviceMemory getIndexBufferMemory();
		std::vector<VkBuffer> getUniformBuffers();
		std::vector<VkDeviceMemory> getUniformBuffersMemories();
		std::vector<void*> getUniformBuffersMapped();
		std::vector<uint16_t> getIndices();
};

#endif