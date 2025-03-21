#include "headers/VulkanApplication.h"

// TODO: Create a Road Map of setting up a general project once The Triangle is completely drawn
// TODO: Try and know the general steps, specific implementation can come later
// TODO: Check for similarities within creating structs and functions (passing in a number then a vector of n size, etc.)
// TODO: Fix Window pausing during resize
// TODO: Does window perform strange behavior during pause?
// TODO: Split entire application into different files:
/*
* Fix the Following Validation Layer Issues
1.
Validation Layer:Validation Error: [ VUID-VkGraphicsPipelineCreateInfo-pDynamicStates-00749 ] | MessageID = 0x1b1ca73 |
vkCreateGraphicsPipelines(): pCreateInfos[0].pRasterizationState->lineWidth is 0.000000, but the line width state is static
(pCreateInfos[0].pDynamicState->pDynamicStates does not contain VK_DYNAMIC_STATE_LINE_WIDTH) and wideLines feature was not enabled.

The Vulkan spec states: If the pipeline requires pre-rasterization shader state, and the wideLines feature is not enabled,
and no element of the pDynamicStates member of pDynamicState is VK_DYNAMIC_STATE_LINE_WIDTH, the lineWidth member of pRasterizationState
must be 1.0 (https://vulkan.lunarg.com/doc/view/1.4.304.1/windows/antora/spec/latest/chapters/pipelines.html#VUID-VkGraphicsPipelineCreateInfo-pDynamicStates-00749)

2.
Validation Layer:Validation Error: [ VUID-vkCmdPipelineBarrier-pImageMemoryBarriers-02820 ] Object 0: handle = 0x24fc1562070,
type = VK_OBJECT_TYPE_COMMAND_BUFFER; | MessageID = 0xaf8d561d | vkCmdPipelineBarrier(): pImageMemoryBarriers[0].dstAccessMask
(VK_ACCESS_TRANSFER_READ_BIT) is not supported by stage mask (VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT).

The Vulkan spec states: For any element of pImageMemoryBarriers, if its srcQueueFamilyIndex and dstQueueFamilyIndex members are equal,
or if its dstQueueFamilyIndex is the queue family index that was used to create the command pool that commandBuffer was allocated from,
then its dstAccessMask member must only contain access flags that are supported by one or more of the pipeline stages in dstStageMask,
as specified in the table of supported access types (https://vulkan.lunarg.com/doc/view/1.4.304.1/windows/antora/spec/latest/chapters/synchronization.html#VUID-vkCmdPipelineBarrier-pImageMemoryBarriers-02820)
*/
/*

	3. Buffer Manager
		- Vertex, Index, and Uniform Buffer Info
		- break out some common function
	3a. Texture Manager
		- Textures can go here
		- Image Loading
	4. Command Manager
		- Command Pool and Command Buffer Info
	7. OpenXR Manager
		- Might be combined into different sections
		- Read OpenXR Official Tutorial for more Info
	8. Camera Object (contains uniform information for uniform buffer)
	10. Sync Object Manager

*/

HelloTriangleApplication::HelloTriangleApplication() {
	initWindow();
	instanceManager = std::make_unique<VulkanApplicationInstanceManager>();
	createSurface();
	deviceManager = std::make_unique<VulkanApplicationDeviceManager>(instanceManager->getInstance(), surface);
	swapchainManager = std::make_unique<VulkanApplicationSwapchainManager>(deviceManager->getPhysicalDevice(), deviceManager->getLogicalDevice(), surface, window);
	graphicsManager = std::make_unique<VulkanApplicationGraphicsManager>(swapchainManager->getSwapchainImageFormat(), deviceManager->getLogicalDevice());
	createDescriptorSetLayout();// descriptor file
	graphicsManager->createGraphicsPipeline(deviceManager->getLogicalDevice(), descriptorSetLayout);
	swapchainManager->createFrameBuffer(deviceManager->getLogicalDevice(), graphicsManager->getRenderPass());
	createCommandPool();		// command
	textureManager = std::make_unique<VulkanApplicationTextureManager>(deviceManager->getLogicalDevice(), deviceManager->getPhysicalDevice(), commandPool, deviceManager->getGraphicsQueue());
	createVertexBuffer();		// buffer
	createIndexBuffer();		// buffer
	createUniformBuffers();		// buffer
	createDescriptorPool();		// descriptor file
	createDescriptorSets();		// descriptor file
	createCommandBuffer();		// command
	createSyncObjects();		// sync
}

HelloTriangleApplication::~HelloTriangleApplication() {
	cleanup();
}

void HelloTriangleApplication::run() {
	mainLoop();
}

void HelloTriangleApplication::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// glfwCreateWindow(width, height, name, monitor, nullptr for vulkan)
	window = glfwCreateWindow(kWIDTH, kHEIGHT, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void HelloTriangleApplication::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}

void HelloTriangleApplication::createDescriptorPool() {
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(kMAX_FRAMES_IN_FLIGHT);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(kMAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(kMAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(deviceManager->getLogicalDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Descriptor Pool");
	}
}

void HelloTriangleApplication::createDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(kMAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(kMAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(kMAX_FRAMES_IN_FLIGHT);

	if (vkAllocateDescriptorSets(deviceManager->getLogicalDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate Descriptor Sets");
	}

	for (size_t i = 0; i < kMAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureManager->getTextureImageView();
		imageInfo.sampler = textureManager->getTextureSampler();

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(deviceManager->getLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void HelloTriangleApplication::createUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(kMAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemories.resize(kMAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(kMAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < kMAX_FRAMES_IN_FLIGHT; i++) {
		createBuffer(deviceManager->getLogicalDevice(), deviceManager->getPhysicalDevice(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemories[i]);

		vkMapMemory(deviceManager->getLogicalDevice(), uniformBuffersMemories[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}
}

void HelloTriangleApplication::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(deviceManager->getLogicalDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Descriptor Set Layout");
	}
}

void HelloTriangleApplication::createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(deviceManager->getLogicalDevice(), deviceManager->getPhysicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(deviceManager->getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t) bufferSize);
	vkUnmapMemory(deviceManager->getLogicalDevice(), stagingBufferMemory);

	createBuffer(deviceManager->getLogicalDevice(), deviceManager->getPhysicalDevice(), bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(deviceManager->getLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(deviceManager->getLogicalDevice(), stagingBufferMemory, nullptr);
}

void HelloTriangleApplication::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(deviceManager->getLogicalDevice(), deviceManager->getPhysicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(deviceManager->getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t) bufferSize);
	vkUnmapMemory(deviceManager->getLogicalDevice(), stagingBufferMemory);

	createBuffer(deviceManager->getLogicalDevice(), deviceManager->getPhysicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(deviceManager->getLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(deviceManager->getLogicalDevice(), stagingBufferMemory, nullptr);
}

void HelloTriangleApplication::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(deviceManager->getLogicalDevice(), commandPool);

	VkBufferCopy copyRegion{};
	// srcOffset and dstOffset are optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer, commandPool, deviceManager->getLogicalDevice(), deviceManager->getGraphicsQueue());
}

void HelloTriangleApplication::createSyncObjects() {
	imageAvailableSemaphores.resize(kMAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(kMAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(kMAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < kMAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(deviceManager->getLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(deviceManager->getLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(deviceManager->getLogicalDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Create Semaphores");
		}
	}
}

void HelloTriangleApplication::createCommandBuffer() {
	commandBuffers.resize(kMAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

	if (vkAllocateCommandBuffers(deviceManager->getLogicalDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Command Buffer");
	}
}

void HelloTriangleApplication::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	// writes commands into command buffer
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Begin Recording Command Buffer");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = graphicsManager->getRenderPass();
	renderPassInfo.framebuffer = (swapchainManager->getSwapchainFramebuffers())[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchainManager->getSwapchainExtent();

	VkClearValue clearColor = { { {0.0f, 0.0f, 0.0f, 1.0f} } };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsManager->getGraphicsPipeline());

	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchainManager->getSwapchainExtent().width);
	viewport.height = static_cast<float>(swapchainManager->getSwapchainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchainManager->getSwapchainExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsManager->getPipelineLayout(),
		0, 1, &descriptorSets[currentFrame], 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Record Command Buffer");
	}
}

void HelloTriangleApplication::createCommandPool() {
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(deviceManager->getPhysicalDevice(), surface);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(deviceManager->getLogicalDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Command Pool");
	}
}

void HelloTriangleApplication::createSurface() {
	if (glfwCreateWindowSurface(instanceManager->getInstance(), window, nullptr, &surface) != VK_SUCCESS) {
		// TODO: MAKE CUSTOM ERROR
		throw std::runtime_error("Failed to Create Window Surface");
	}
}

void HelloTriangleApplication::mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(deviceManager->getLogicalDevice());
}

void HelloTriangleApplication::drawFrame() {
	// double check semaphores and fences
	// semaphore for swapchain, fence for waiting on previous frame (forces host to wait)
	// device, num fence, fences, wait on all?, uint64 removes the timeout
	// make sure that the fence is signaled on creation or will stick here
	vkWaitForFences(deviceManager->getLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(deviceManager->getLogicalDevice(), swapchainManager->getSwapchain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		swapchainManager->recreateSwapchain(deviceManager->getPhysicalDevice(), deviceManager->getLogicalDevice(), surface, window, graphicsManager->getRenderPass());
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to Acquire Swapchain Image");
	}

	vkResetFences(deviceManager->getLogicalDevice(), 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);
	updateUniformBuffer(currentFrame);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	// what semaphore to wait on, what pipeline stage to wait on, num semaphore
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(deviceManager->getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Submit Draw Command");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapchains[] = { swapchainManager->getSwapchain()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(deviceManager->getPresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		swapchainManager->recreateSwapchain(deviceManager->getPhysicalDevice(), deviceManager->getLogicalDevice(), surface, window, graphicsManager->getRenderPass());
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to Present Swapchain Image");
	}

	currentFrame = (currentFrame + 1) % kMAX_FRAMES_IN_FLIGHT;
}

void HelloTriangleApplication::updateUniformBuffer(uint32_t currentImage) {
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.projection = glm::perspective(glm::radians(45.0f), (swapchainManager->getSwapchainExtent().width / (float)swapchainManager->getSwapchainExtent().height), 0.1f, 10.0f);

	ubo.projection[1][1] *= -1; // flip y since vulkan is upside down
	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void HelloTriangleApplication::cleanup() {
	// done automatically -> vkFreeCommandBuffers(deviceManager->getLogicalDevice(), commandPool, 1, &commandBuffer);
	swapchainManager->cleanup(deviceManager->getLogicalDevice());
	textureManager->cleanup(deviceManager->getLogicalDevice());

	vkDestroyDescriptorPool(deviceManager->getLogicalDevice(), descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(deviceManager->getLogicalDevice(), descriptorSetLayout, nullptr);

	for (size_t i = 0; i < kMAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(deviceManager->getLogicalDevice(), uniformBuffers[i], nullptr);
		vkFreeMemory(deviceManager->getLogicalDevice(), uniformBuffersMemories[i], nullptr);
	}

	vkDestroyBuffer(deviceManager->getLogicalDevice(), indexBuffer, nullptr);
	vkFreeMemory(deviceManager->getLogicalDevice(), indexBufferMemory, nullptr);
	vkDestroyBuffer(deviceManager->getLogicalDevice(), vertexBuffer, nullptr);
	vkFreeMemory(deviceManager->getLogicalDevice(), vertexBufferMemory, nullptr);

	graphicsManager->cleanup(deviceManager->getLogicalDevice());

	for (size_t i = 0; i < kMAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(deviceManager->getLogicalDevice(), imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(deviceManager->getLogicalDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(deviceManager->getLogicalDevice(), inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(deviceManager->getLogicalDevice(), commandPool, nullptr);

	deviceManager->cleanup();

	// nullptr is a custom allocator callback
	vkDestroySurfaceKHR(instanceManager->getInstance(), surface, nullptr); //destroy before instance

	instanceManager->cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();
}