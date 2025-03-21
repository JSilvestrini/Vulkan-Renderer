#ifndef TRIANGLE_APPLICATION
#define TRIANGLE_APPLICATION

#include "VulkanApplicationInstanceManager.h"
#include "VulkanApplicationDeviceManager.h"
#include "VulkanApplicationSwapchainManager.h"
#include "VulkanApplicationGraphicsManager.h"
#include "VulkanApplicationTextureManager.h"

#include <chrono>

using std::cout, std::cerr, std::endl;

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
		std::unique_ptr<VulkanApplicationGraphicsManager> graphicsManager;
		std::unique_ptr<VulkanApplicationTextureManager> textureManager;

		// command file
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
		// sync object file
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		// this one (for now)
		uint32_t currentFrame = 0;
		bool framebufferResized = false;
		// buffer file
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		// descriptor file
		VkDescriptorSetLayout descriptorSetLayout;
		// buffer
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemories;
		std::vector<void*> uniformBuffersMapped;
		// descriptor
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;


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
		void createSurface();
		void createSyncObjects();
		void createCommandBuffer();
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void createCommandPool();

		void createDescriptorSetLayout();

		void mainLoop();
		void drawFrame();
		void cleanup();

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void createVertexBuffer();
		void createIndexBuffer();

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void createUniformBuffers();
		void updateUniformBuffer(uint32_t currentImage);
		void createDescriptorPool();
		void createDescriptorSets();




};

#endif