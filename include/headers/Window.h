#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(int width, int height, const char* title);
	~Window();
	void makeContextCurrent();
	void swapBuffers();
	bool shouldClose() const;
	void pollEvents() const;
	void setFramebufferSizeCallback(GLFWframebuffersizefun callback) const;
	int getWidth() const;
	int getHeight() const;
	const char* getTitle() const;
	GLFWwindow* getGLFWwindow() const;
	void getFramebufferSize(int& width, int& height) const;

private:
	GLFWwindow* m_window;
	int m_width, m_height;
	const char* m_title;
};