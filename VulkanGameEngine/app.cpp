#include "app.hpp"

//std
#include <stdexcept>
#include <array>
namespace vlk {

	App::App() {
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	App::~App() {
		vkDestroyPipelineLayout(vlkDevice.device(), pipelineLayout, nullptr);
	}
	void App::run() {
		while (!vlkWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(vlkDevice.device());
	}
	void App::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(vlkDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void App::createPipeline() {
		assert(vlkSwapChain != nullptr && "Cannot create pipeline before swapchain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		//auto pipelineConfig = VLKPipeline::defaultPipelineConfigInfo(vlkSwapChain->width(), vlkSwapChain->height());
		PipelineConfigInfo pipelineConfig{};
		VLKPipeline::defaultPipelineConfigInfo(
			pipelineConfig);
		pipelineConfig.renderPass = vlkSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		vlkPipeline = std::make_unique<VLKPipeline>(
			vlkDevice,
			"C:/Users/trung/Documents/Project/VulkanGameEngine/VulkanGameEngine/shader1.vert.spv",
			"C:/Users/trung/Documents/Project/VulkanGameEngine/VulkanGameEngine/shader1.frag.spv",
			pipelineConfig
		);
	}

	void App::createCommandBuffers(){
		commandBuffers.resize(vlkSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vlkDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(vlkDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	void App::freeCommandBuffers() {
		vkFreeCommandBuffers(
			vlkDevice.device(),
			vlkDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}


	void App::drawFrame(){
		uint32_t imageIndex;
		auto result = vlkSwapChain->acquireNextImage(&imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		recordCommandBuffer(imageIndex);
		result = vlkSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vlkWindow.wasWindowResized()) {
			vlkWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
	}

	void App::loadModels() {
		std::vector<VLKModel::Vertex> vertices{
			{{ -0.5f, 0.5f }, {1.0f, 0.0f, 0.0f}},
			{{ 0.5f, 0.5f }, {0.0f, 1.0f, 0.0f}},
			{{ 0.0f, -0.5f }, {0.0f, 0.0f, 1.0f}}
		};
		
		vlkModel = std::make_unique<VLKModel>(vlkDevice, vertices);
	}

	void App::recreateSwapChain() {
		auto extent = vlkWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = vlkWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(vlkDevice.device());

		if (vlkSwapChain == nullptr){
			vlkSwapChain = std::make_unique<VLKSwapChain>(vlkDevice, extent);
		}
		else {
			vlkSwapChain = std::make_unique<VLKSwapChain>(vlkDevice, extent, std::move(vlkSwapChain));
			if (vlkSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		createPipeline();
	}

	void App::recordCommandBuffer(int imageIndex){
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vlkSwapChain->getRenderPass();
		renderPassInfo.framebuffer = vlkSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vlkSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 0.1f };
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(vlkSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(vlkSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, vlkSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		vlkPipeline->bind(commandBuffers[imageIndex]);
		vlkModel->bind(commandBuffers[imageIndex]);
		vlkModel->draw(commandBuffers[imageIndex]);
		//vkCmdDraw(commandBuffers[imageIndex], 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}
	}
}
