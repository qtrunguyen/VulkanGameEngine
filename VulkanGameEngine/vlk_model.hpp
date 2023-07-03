#pragma once

#include "vlk_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
namespace vlk {
	class VLKModel {
	public:

		struct Vertex {
			glm::vec2 position;
			glm::vec3 color;
			static std::vector<VkVertexInputBindingDescription> getBindingDescription();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();
		};

		VLKModel(VLKDevice &device, const std::vector<Vertex> &vertices);
		~VLKModel();

		VLKModel(const VLKModel&) = delete;
		VLKModel &operator=(const VLKModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		VLKDevice& vlkDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}