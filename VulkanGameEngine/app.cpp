#include "app.hpp"

//std
#include <stdexcept>
#include <array>
namespace vlk {

	App::App() {
		loadModels();
		createPipelineLayout();
		createPipeline();
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
		//auto pipelineConfig = VLKPipeline::defaultPipelineConfigInfo(vlkSwapChain.width(), vlkSwapChain.height());
		PipelineConfigInfo pipelineConfig{};
		VLKPipeline::defaultPipelineConfigInfo(
			pipelineConfig,
			vlkSwapChain.width(),
			vlkSwapChain.height());
		pipelineConfig.renderPass = vlkSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		vlkPipeline = std::make_unique<VLKPipeline>(
			vlkDevice,
			"C:/Users/trung/Documents/Project/VulkanGameEngine/VulkanGameEngine/shader1.vert.spv",
			"C:/Users/trung/Documents/Project/VulkanGameEngine/VulkanGameEngine/shader1.frag.spv",
			pipelineConfig
		);
	}

	void App::createCommandBuffers(){
		commandBuffers.resize(vlkSwapChain.imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vlkDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(vlkDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}

		for (int i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = vlkSwapChain.getRenderPass();
			renderPassInfo.framebuffer = vlkSwapChain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = vlkSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 0.1f };
			clearValues[1].depthStencil = {1.0f, 0};

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vlkPipeline->bind(commandBuffers[i]);
			vlkModel->bind(commandBuffers[i]);
			vlkModel->draw(commandBuffers[i]);
			//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer");
			}
		}

	}
	void App::drawFrame(){
		uint32_t imageIndex;
		auto result = vlkSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		result = vlkSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
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

	
}
