#include "headers/Window.h"
#include <iostream>
#include <stdexcept>

// Initialize GLFW, configure the OpenGL context, and create the application window.
Window::Window(int width, int height, const char* title)
	: m_width(width), m_height(height), m_title(title)
{
	// Initialize the GLFW library before attempting to create a window.
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}

	// Request an OpenGL 3.3 context using the core profile.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the GLFW window using the requested dimensions and title.
	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	
	// Terminate GLFW if window creation fails.
	if (!m_window)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	// Activate the newly created window's OpenGL context.
	makeContextCurrent();

	// Load the OpenGL function pointers required for rendering.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
		glfwTerminate();
	}
}

// Release GLFW resources when the Window object goes out of scope.
Window::~Window()
{
	glfwTerminate();
}

// Set this window's OpenGL context as the active context.
void Window::makeContextCurrent()
{
	glfwMakeContextCurrent(m_window);
}

// Swap the front and back buffers to display the rendered frame.
void Window::swapBuffers()
{
	glfwSwapBuffers(m_window);
}

// Return whether the application window should be closed.
bool Window::shouldClose() const
{
	return glfwWindowShouldClose(m_window);
}

// Forward event processing to GLFW.
void Window::pollEvents() const
{
	glfwPollEvents();
}

// Attach the provided framebuffer resize callback to this window.
void Window::setFramebufferSizeCallback(GLFWframebuffersizefun callback) const
{
	glfwSetFramebufferSizeCallback(m_window, callback);
}

// Return the width originally assigned to the window.
int Window::getWidth() const
{
	return m_width;
}

// Return the height originally assigned to the window.
int Window::getHeight() const
{
	return m_height;
}

// Return the title associated with the window.
const char* Window::getTitle() const
{
	return m_title;
}

// Expose the underlying GLFW handle for operations requiring direct window access.
GLFWwindow* Window::getWindow() const
{
	return m_window;
}

// Query the actual framebuffer dimensions, accounting for display scaling.
void Window::getFramebufferSize(int& width, int& height) const
{
	glfwGetFramebufferSize(m_window, &width, &height);
}
