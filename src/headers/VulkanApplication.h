#ifndef TRIANGLE_APPLICATION
#define TRIANGLE_APPLICATION

#include "VulkanApplicationInstanceManager.h"
#include "VulkanApplicationDeviceManager.h"
#include "VulkanApplicationSwapchainManager.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <fstream>

#include <vector>
#include <optional>
#include <set>
#include <array>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include <stb_image.h>

using std::cout, std::cerr, std::endl;

struct Vertex {
	glm::vec3 color;
	glm::vec3 pos;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

// TODO: Create a Road Map of setting up a general project once The Triangle is completely drawn
// TODO: Try and know the general steps, specific implementation can come later
// TODO: Check for similarities within creating structs and functions (passing in a number then a vector of n size, etc.)

class HelloTriangleApplication {
	private:
		GLFWwindow* window;
		std::unique_ptr<VulkanApplicationInstanceManager> instanceManager;
		VkSurfaceKHR surface; // Could use platform specific stuff here if I wanted
		std::unique_ptr<VulkanApplicationDeviceManager> deviceManager;
		std::unique_ptr<VulkanApplicationSwapchainManager> swapchainManager;

		// graphics pipeline file/ render file
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;

		// command file
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

		// sync object file
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		uint32_t currentFrame = 0;
		bool framebufferResized = false;

		// buffer file
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemories;
		std::vector<void*> uniformBuffersMapped;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;

		//texture file
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;

		const std::vector<Vertex> vertices = {
			{{1.0f, 0.0f, 0.0f}, {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
			{{0.0f, 1.0f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
			{{0.0f, 0.0f, 1.0f}, {0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
			{{1.0f, 1.0f, 1.0f}, {-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}}
		};

		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
		};

	public:
		HelloTriangleApplication();
		~HelloTriangleApplication();
		void run();
	private:
		void initWindow();
		void initVulkan();
		void createSurface();
		void createSyncObjects();
		void createCommandBuffer();
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void createCommandPool();
		void createRenderPass();
		void createDescriptorSetLayout();
		void createGraphicsPipeline();
		VkShaderModule createShaderModule(const std::vector<char>& code);
		static std::vector<char> readFile(const std::string& filename);

		void mainLoop();
		void drawFrame();
		void cleanup();

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void createVertexBuffer();
		void createIndexBuffer();
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void createUniformBuffers();
		void updateUniformBuffer(uint32_t currentImage);
		void createDescriptorPool();
		void createDescriptorSets();
		void createTextureImage();
		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image,
			VkDeviceMemory& imageMemory);
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		VkCommandBuffer beginSingleTimeCommands();
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void createTextureImageView();
		void createTextureSampler();
};

#endif