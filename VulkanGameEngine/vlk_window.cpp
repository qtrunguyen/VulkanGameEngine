#include "vlk_window.hpp"

#include <stdexcept>
namespace vlk {
	VLKWindow::VLKWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name }{
		initWindow();
	}

	VLKWindow::~VLKWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VLKWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);

	}
	void VLKWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void VLKWindow::frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto vlkWindow = reinterpret_cast<VLKWindow*>(glfwGetWindowUserPointer(window));
		vlkWindow->framebufferResized = true;
		vlkWindow->width = width;
		vlkWindow->height = height;
	}
}