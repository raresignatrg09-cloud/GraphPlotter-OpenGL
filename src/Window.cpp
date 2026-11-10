#include "headers/Window.h"
#include <iostream>

Window::Window(int width, int height, const char* title)
	: m_width(width), m_height(height), m_title(title)
{
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!m_window)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	makeContextCurrent();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}
}

Window::~Window()
{
	glfwTerminate();
}

void Window::makeContextCurrent()
{
	glfwMakeContextCurrent(m_window);
}

void Window::swapBuffers()
{
	glfwSwapBuffers(m_window);
}

bool Window::shouldClose() const
{
	return glfwWindowShouldClose(m_window);
}

void Window::pollEvents() const
{
	glfwPollEvents();
}

void Window::setFramebufferSizeCallback(GLFWframebuffersizefun callback) const
{
	glfwSetFramebufferSizeCallback(m_window, callback);
}

int Window::getWidth() const
{
	return m_width;
}

int Window::getHeight() const
{
	return m_height;
}

const char* Window::getTitle() const
{
	return m_title;
}

GLFWwindow* Window::getGLFWwindow() const
{
	return m_window;
}

void Window::getFramebufferSize(int& width, int& height) const
{
	glfwGetFramebufferSize(m_window, &width, &height);
}
