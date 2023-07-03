#pragma once

#include "vlk_window.hpp"
#include "vlk_pipeline.hpp"
#include "vlk_device.hpp"
#include "vlk_swap_chain.hpp"
#include "vlk_model.hpp"

//std
#include <memory>
#include <vector>
namespace vlk {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		void run();
	private:
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		VLKWindow vlkWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		VLKDevice vlkDevice{ vlkWindow };
		VLKSwapChain vlkSwapChain{ vlkDevice, vlkWindow.getExtent() };
		std::unique_ptr<VLKPipeline> vlkPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<VLKModel> vlkModel;
		/*VLKPipeline vlkPipeline{vlkDevice, 
								"C:/Users/trung/Documents/Project/VulkanGameEngine/VulkanGameEngine/shader1.vert.spv",
								"C:/Users/trung/Documents/Project/VulkanGameEngine/VulkanGameEngine/shader1.frag.spv",
								VLKPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};*/
	};
}