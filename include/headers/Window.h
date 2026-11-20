#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Encapsulate GLFW window management and expose the functionality needed by the application.
class Window
{
public:
    // Initialize a window with the specified dimensions and title.
    Window(int width, int height, const char* title);

    // Release the resources associated with the windowing system.
    ~Window();

    // Make this window's OpenGL context the current context.
    void makeContextCurrent();

    // Present the rendered frame by swapping the front and back buffers.
    void swapBuffers();

    // Check whether the window has received a close request.
    bool shouldClose() const;

    // Process pending window and input events.
    void pollEvents() const;

    // Register a callback that is invoked whenever the framebuffer is resized.
    void setFramebufferSizeCallback(GLFWframebuffersizefun callback) const;

    // Retrieve the dimensions originally supplied when creating the window.
    int getWidth() const;
    int getHeight() const;

    // Retrieve the window's title.
    const char* getTitle() const;

    // Provide access to the underlying GLFW window handle.
    GLFWwindow* getWindow() const;

    // Retrieve the current framebuffer dimensions, which may differ from the window dimensions.
    void getFramebufferSize(int& width, int& height) const;

private:
    // Store the GLFW window handle and its original configuration.
    GLFWwindow* m_window;
    int m_width, m_height;
    const char* m_title;
};