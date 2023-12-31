#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
namespace vlk {
	class VLKWindow{
	public:
		VLKWindow(int w, int h, std::string name);
		~VLKWindow();

		VLKWindow(const VLKWindow&) = delete;
		VLKWindow& operator=(const VLKWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }

		VkExtent2D getExtent () { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool framebufferResized = false;
		std::string windowName;
		GLFWwindow* window;
		
	};
}
